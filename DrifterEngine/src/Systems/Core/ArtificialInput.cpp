#include "pch.h"
#include "ArtificialInput.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Random/RandomNumberGenerator.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Helpers/HasLineOfSight.h"
#include "GOAP/Plan.h"
#include "GOAP/GoalRegistry.h"
#include "GOAP/ActionRegistry.h"


void drft::system::ArtificialInput::init()
{
	goap::ActionRegistry::bind();
	goap::GoalRegistry::bind();
}

void drft::system::ArtificialInput::update(const float dt)
{
	auto view = registry->view<component::AI, const component::Position, component::tag::CurrentActor>();
	for (auto [entity, ai, myPos] : view.each())
	{
		switch (ai.state)
		{
		case AIState::Think:
			aiThink(ai);
			break;
		case AIState::MoveTo:
			aiMoveTo(ai);
			break;
		case AIState::PerformAction:
			aiPerformAction(ai);
			break;
		}
	}
}

bool drft::system::ArtificialInput::inSightRange(sf::Vector2i position, const component::AI& ai) const
{
	auto handle = getHandle(ai);
	auto myPosition = handle.get<component::Position>().position;
	if (spatial::distance(myPosition, position) < ai.sightRange) return true;
	return false;
}

void drft::system::ArtificialInput::randomMove(entt::handle entity) const
{
	int randx = rng::RandomNumberGenerator::intInRange(-1, 1);
	int randy = rng::RandomNumberGenerator::intInRange(-1, 1);
	const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
	const auto& tilepos = entity.get<component::Position>().position;

	auto blockerFilter = [this](entt::entity entity) -> bool
	{
		if (auto physical = registry->try_get<component::Physical>(entity))
		{
			return physical->blocks;
		}
		return false;
	};

	auto blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);
	
	int safetyCount = 0; // in case entity is surrounded
	while (safetyCount < 8 && !blockers.empty())
	{
		randx = rng::RandomNumberGenerator::intInRange(-1, 1);
		randy = rng::RandomNumberGenerator::intInRange(-1, 1);
		blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);
		++safetyCount;
	}
	
	entity.emplace<component::action::Move>(sf::Vector2i(randx, randy));
}

void drft::system::ArtificialInput::moveToTarget(entt::handle entity, sf::Vector2i targetPosition) const
{
	auto& position = entity.get<component::Position>().position;
	auto line = spatial::getIntPointsAlongLine(position, targetPosition);
	sf::Vector2i delta;
	if (line.empty())
	{
		delta = position - targetPosition;
	}
	else
	{
		delta = position - line.front();
	}
	int xMove = delta.x == 0 ? 0 : -(delta.x / abs(delta.x));
	int yMove = delta.y == 0 ? 0 : -(delta.y / abs(delta.y));

	entity.emplace<component::action::Move>(sf::Vector2i{ xMove, yMove });
}

void drft::system::ArtificialInput::pathToTarget(entt::handle entity, sf::Vector2i targetPosition) const
{
	auto& position = entity.get<component::Position>().position;
	if (!_cachedPaths.contains(entity.entity()) || _cachedPaths.at(entity.entity()).empty())
	{
		const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
		_cachedPaths[entity.entity()] = grid.getPath(position, targetPosition,
			[this](const std::vector<entt::entity>& entities) -> int
			{
				for (auto entity : entities)
				{
					if (auto physical = registry->try_get<component::Physical>(entity))
					{
						if (physical->blocks) return 1000;
					}
				}
				return 0;
			});
	}
	if (_cachedPaths.at(entity.entity()).empty())
	{
		moveToTarget(entity, targetPosition);
	}
	else
	{
		moveToTarget(entity, _cachedPaths.at(entity.entity()).front());
		_cachedPaths.at(entity.entity()).pop_front();
	}
}

void drft::system::ArtificialInput::clearPathCache(entt::entity entity) const
{
	if (!_cachedPaths.contains(entity)) return;
	_cachedPaths.erase(entity);
}

entt::handle drft::system::ArtificialInput::getHandle(component::AI& ai)
{
	entt::entity entity = entt::to_entity(*registry, ai);
	entt::handle aiHandle = { *registry, entity };
	return aiHandle;
}

entt::const_handle drft::system::ArtificialInput::getHandle(const component::AI& ai) const
{
	entt::entity entity = entt::to_entity(*registry, ai);
	entt::handle aiHandle = { *registry, entity };
	return aiHandle;
}

void drft::system::ArtificialInput::generatePlan(component::AI& ai) const
{
	auto goals = prioritizeGoals(ai);
	if (!ai.plan.empty())
	{
		const auto& lastAction = goap::ActionRegistry::get(ai.plan.back());
		const auto& firstAction = goap::ActionRegistry::get(ai.plan.front());
		if (firstAction.isValid(ai.blackboard) 
			&& lastAction.effects().isSupersetOf(goals.top())) return;
	}

	while (!goals.empty())
	{
		auto tempPlan = goap::plan(ai.blackboard, getAiActions(ai), goals.top());
		goals.pop();
		if (!tempPlan) continue;
		const auto& firstAction = goap::ActionRegistry::get(tempPlan.value().front());
		if (firstAction.isValid(ai.blackboard))
		{
			ai.plan = tempPlan.value();
			return;
		}
	}
	// No plan constructed

}

std::stack<std::reference_wrapper<const drft::goap::WorldState>> drft::system::ArtificialInput::prioritizeGoals(const component::AI& ai) const
{
	std::stack<std::reference_wrapper<const goap::WorldState>> result;
	for (auto&& goal : ai.goals)
	{
		result.push(std::ref( goap::GoalRegistry::get(goal)));
	}
	return result;
}

std::unordered_set<drft::goap::AiAction> drft::system::ArtificialInput::getAiActions(const component::AI& ai) const
{
	std::unordered_set<goap::AiAction> result;
	entt::const_handle entity = { *registry, entt::to_entity(*registry, ai) };
	if (entity.all_of<component::Faction>())
	{
		result.insert(goap::AiAction::SpotHostile);
		result.insert(goap::AiAction::AttackHostile);
		result.insert(goap::AiAction::RunFromHostile);
	}

	return result;
}

void drft::system::ArtificialInput::aiThink(component::AI& ai)
{
	

	// Run world sensors

	generatePlan(ai);
	
	if (ai.plan.empty()) 
	{
		randomMove(aiHandle);
	}
	else
	{
		ai.state = AIState::MoveTo;
	}
}

void drft::system::ArtificialInput::aiMoveTo(component::AI& ai)
{
	auto handle = getHandle(ai);
	const auto& action = goap::ActionRegistry::get(ai.plan.front());
	if (!action.isValid(ai.blackboard))
	{
		ai.state = AIState::Think;
		return;
	}
	if (!action.requiresInRange() || action.isInRange(handle))
	{
		ai.state = AIState::PerformAction;
	}
	else
	{
		auto optionalTarget = action.setMoveTarget(handle);
		if (optionalTarget.has_value() && inSightRange(optionalTarget.value(), ai))
		{
			if (hasLineOfSight(handle, optionalTarget.value()))
			{
				clearPathCache(handle.entity());
				moveToTarget(handle, optionalTarget.value());
			}
			else
			{
				pathToTarget(handle, optionalTarget.value());
			}
		}
		else
		{
			clearPathCache(handle.entity());
			ai.state = AIState::Think;
		}
	}
}

void drft::system::ArtificialInput::aiPerformAction(component::AI& ai)
{
	if (ai.plan.empty())
	{
		ai.target = entt::null;
		ai.state = AIState::Think;
		return;
	}

	const auto& action = goap::ActionRegistry::get(ai.plan.front());
	if (!action.isValid(ai.blackboard))
	{
		ai.state = AIState::Think;
		return;
	}

	action.perform(entt::handle{ *registry, entt::to_entity(*registry, ai) }, entt::handle{ *registry, ai.target });
	ai.plan.pop_front();
	ai.target = entt::null;
	ai.state = AIState::Think;
}
