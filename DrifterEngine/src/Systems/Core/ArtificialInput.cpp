#include "pch.h"
#include "ArtificialInput.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Random/RandomNumberGenerator.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Gameplay/TargetSelectors/TargetSelectors.h"
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

entt::entity drft::system::ArtificialInput::findTarget(entt::handle entity, std::function<bool(entt::const_handle, entt::const_handle)> selector) const
{
	if (!entity.all_of<component::Faction>()) return entt::null;

	const auto& ai = entity.get<component::AI>();
	const auto& pos = entity.get<component::Position>();

	float closestRange = static_cast<float>(ai.sightRange);
	entt::entity closestTarget = entt::null;

	auto view = registry->view<const component::Position, component::tag::Active>();
	for (auto [otherEntity, otherPos] : view.each())
	{
		const float distance = spatial::distance(pos.position, otherPos.position);
		if (distance < ai.sightRange && distance < closestRange)
		{
			entt::const_handle otherHandle = { *registry, otherEntity };
			if (hasLineOfSight(pos.position, otherPos.position) && selector(entity, otherHandle))
			{
				closestRange = distance;
				closestTarget = otherEntity;
			}
		}
	}
	
	return closestTarget;
}

bool drft::system::ArtificialInput::hasLineOfSight(sf::Vector2i myPosition, sf::Vector2i targetPosition) const
{
	auto tilesInLOS = spatial::getIntPointsAlongLine(myPosition, targetPosition);
	const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
	for (auto tile : tilesInLOS)
	{
		auto entities = grid.entitiesAt(tile,
			[this](entt::entity entity) -> bool
			{
				const bool blocksLight = registry->any_of<component::LightBlocking>(entity);
				return blocksLight;
			});

		if (!entities.empty()) return false;
	}

	return true;
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

void drft::system::ArtificialInput::moveToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const
{
	auto line = spatial::getIntPointsAlongLine(myPosition, targetPosition);
	sf::Vector2i delta;
	if (line.empty())
	{
		delta = myPosition - targetPosition;
	}
	else
	{
		delta = myPosition - line.front();
	}
	int xMove = delta.x == 0 ? 0 : -(delta.x / abs(delta.x));
	int yMove = delta.y == 0 ? 0 : -(delta.y / abs(delta.y));

	registry->emplace<component::action::Move>(ai, sf::Vector2i{ xMove, yMove });
}

void drft::system::ArtificialInput::pathToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const
{
	if (!_cachedPaths.contains(ai) || _cachedPaths.at(ai).empty())
	{
		const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
		_cachedPaths[ai] = grid.getPath(myPosition, targetPosition, 
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
	if (_cachedPaths.at(ai).empty())
	{
		moveToTarget(ai, myPosition, targetPosition);
	}
	else
	{
		moveToTarget(ai, myPosition, _cachedPaths.at(ai).front());
		_cachedPaths.at(ai).pop_front();
	}
}

void drft::system::ArtificialInput::clearPathCache(entt::entity entity) const
{
	if (!_cachedPaths.contains(entity)) return;
	_cachedPaths.erase(entity);
}

bool drft::system::ArtificialInput::isTargetValid(component::AI& ai) const
{
	if (ai.target == entt::null || !registry->valid(ai.target))
	{
		ai.target = entt::null;
		ai.state = AIState::Think;
		return false;
	}
	return true;
}

void drft::system::ArtificialInput::generatePlan(component::AI& ai) const
{
	auto goals = prioritizeGoals(ai);
	if (!ai.plan.empty())
	{
		// Check if already pursuing this goal...
		const auto& lastAction = goap::ActionRegistry::get(ai.plan.back());
		if (lastAction.effects().isSupersetOf(goals.top())) return;
	}

	while (!goals.empty())
	{
		auto tempPlan = goap::plan(ai.blackboard, getAiActions(ai), goals.top());
		goals.pop();
		if (!tempPlan) continue;
		const auto& firstAction = goap::ActionRegistry::get(tempPlan.value().front());
		if (ai.blackboard.isSupersetOf(firstAction.preconditions()))
		{
			ai.plan = tempPlan.value();
			return;
		}
	}
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
	entt::entity entity = entt::to_entity(*registry, ai);
	entt::handle eHandle = { *registry, entity };

	// Run world sensors

	generatePlan(ai);
	
	if (ai.plan.empty()) 
	{
		randomMove(eHandle);
		return;
	}

	ai.target = findTarget(eHandle, [&ai](auto actor, auto target) -> bool {
			return goap::ActionRegistry::get(ai.plan.value().front()).isValidTarget(actor, target);});

	if (ai.target != entt::null)
	{
		ai.state = AIState::MoveTo;
	}
	else
	{
		// If no performable action random move
		randomMove(eHandle);
	}
}

void drft::system::ArtificialInput::aiMoveTo(component::AI& ai)
{
	if (!isTargetValid(ai)) return;

	entt::entity entity = entt::to_entity(*registry, ai);
	auto& targetPos = registry->get<component::Position>(ai.target);
	auto& myPos = registry->get<component::Position>(entity);

	// if (action.isInRange(pos, targetPos))
	//		ai.state = AIState::PerformAction;
	//		return;

	if (spatial::distance(myPos.position, targetPos.position) <= ai.sightRange)
	{
		if (hasLineOfSight(myPos.position, targetPos.position))
		{
			clearPathCache(entity);
			moveToTarget(entity, myPos.position, targetPos.position);
		}
		else
		{
			pathToTarget(entity, myPos.position, targetPos.position);
		}
	}
	else
	{
		clearPathCache(entity);
		ai.target = entt::null;
		ai.state = AIState::Think;
	}
}

void drft::system::ArtificialInput::aiPerformAction(component::AI& ai)
{
	if (!isTargetValid(ai)) return;
	if (ai.plan.empty())
	{
		ai.target = entt::null;
		ai.state = AIState::Think;
		return;
	}
	const auto& action = goap::ActionRegistry::get(ai.plan.front());
	action.perform(entt::handle{ *registry, entt::to_entity(*registry, ai) }, entt::handle{ *registry, ai.target });
	ai.plan.pop_front();
	ai.target = entt::null;
	ai.state = AIState::Think;
}
