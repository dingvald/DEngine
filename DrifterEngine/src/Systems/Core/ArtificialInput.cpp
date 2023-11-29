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

#include "GOAP/Sensors/HostileSensor.h"


void drft::system::ArtificialInput::init()
{
	goap::ActionRegistry::bind();
	goap::GoalRegistry::bind();

	_sensorySystem.registerSensor(std::make_unique<goap::HostileSensor>());
	_sensorySystem.registerChecker(hasLineOfSight, goap::SensorType::Visual);
}

void drft::system::ArtificialInput::update(const float dt)
{
	auto view = registry->view<component::AI, const component::Position, component::tag::CurrentActor>();
	for (auto [entity, ai, myPos] : view.each())
	{
		senseWorldState(ai);
		executeStateNow(ai, ai.state);
	}
}


bool drft::system::ArtificialInput::inSightRange(sf::Vector2i position, const component::AI& ai) const
{
	auto myPosition = registry->get<component::Position>(entt::to_entity(*registry, ai)).position;
	if (spatial::distance(myPosition, position) < ai.sightRange) return true;
	return false;
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

entt::handle drft::system::ArtificialInput::getHandle(component::AI& ai) const
{
	entt::entity entity = entt::to_entity(*registry, ai);
	entt::handle aiHandle = { *registry, entity };
	return aiHandle;
}

void drft::system::ArtificialInput::senseWorldState(component::AI& ai) 
{
	ai.entitiesOfInterest.clear();
	_sensorySystem.runSensors(getHandle(ai));
}

std::deque<drft::goap::AiAction> drft::system::ArtificialInput::generatePlan(const component::AI& ai, const goap::Goal& exclude) const
{
	auto goals = prioritizeGoals(ai);
	std::optional<std::deque<goap::AiAction>> optionalPlan;
	while (!goals.empty())
	{
		optionalPlan = goap::plan(ai.blackboard, getAiActions(ai), goals.front());
		goals.pop();
		if (optionalPlan.has_value())
		{
			return optionalPlan.value();
		}
	}
	return std::deque<goap::AiAction>{goap::AiAction::RandomMove};
}

bool drft::system::ArtificialInput::isPlanValid(const goap::WorldState& worldState, const goap::Plan& plan, const goap::Goal& goal) const
{
	if (plan.empty()) return false;
	const auto& firstAction = goap::ActionRegistry::get(plan.front());
	const auto& lastAction = goap::ActionRegistry::get(plan.back());
	if (worldState.contains(firstAction.preconditions())
		&& lastAction.effects().contains(goal))
	{
		return true;
	}
	return false;
}

int drft::system::ArtificialInput::calculateGoalValue(const goap::Goal& goal, const component::AI& ai) const
{
	// TODO: This is just placeholder

	if (goal.contains(goap::WorldState{ {goap::look_busy, true} }))
	{
		return -1;
	}
	return 1;
}

std::queue<drft::goap::Goal> drft::system::ArtificialInput::prioritizeGoals(const component::AI& ai, const goap::Goal& exclude) const
{
	std::queue<goap::Goal> result;
	std::map<int, goap::Goal, std::greater<int>> ranking;
	for (auto&& goalName : ai.goals)
	{
		const auto& goal = goap::GoalRegistry::get(goalName);
		if (goal.isSameAs(exclude)) continue;
		int value = calculateGoalValue(goal, ai);
		ranking.emplace(value, goal);
	}
	for (auto&& [_, goal] : ranking)
	{
		result.push(goal);
	}

	return result;
}

std::unordered_set<drft::goap::AiAction> drft::system::ArtificialInput::getAiActions(const component::AI& ai) const
{
	std::unordered_set<goap::AiAction> result;
	entt::const_handle entity = { *registry, entt::to_entity(*registry, ai) };
	result.insert(goap::AiAction::RandomMove);

	if (entity.all_of<component::Faction>())
	{
		result.insert(goap::AiAction::SpotHostile);
		result.insert(goap::AiAction::AttackHostile);
		result.insert(goap::AiAction::EscapeHostiles);
	}

	return result;
}

drft::goap::Goal drft::system::ArtificialInput::getCurrentGoal(const component::AI& ai) const
{
	if (ai.plan.empty()) return {};
	return goap::ActionRegistry::get(ai.plan.back()).effects();
}

void drft::system::ArtificialInput::setNextState(component::AI& ai, AIState state) const
{
	ai.state = state;
}

void drft::system::ArtificialInput::executeStateNow(component::AI& ai, AIState state) const
{
	ai.state = state;
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


// AI State Machine

void drft::system::ArtificialInput::aiThink(component::AI& ai) const
{
	if (!isPlanValid(ai.blackboard, ai.plan, getCurrentGoal(ai)))
	{
		ai.plan = generatePlan(ai);
	}
	executeStateNow(ai, AIState::MoveTo);
}

void drft::system::ArtificialInput::aiMoveTo(component::AI& ai) const
{
	if (!isPlanValid(ai.blackboard, ai.plan, getCurrentGoal(ai)))
	{
		executeStateNow(ai, AIState::Think);
		return;
	}

	auto handle = getHandle(ai);
	const goap::IAction& action = goap::ActionRegistry::get(ai.plan.front());

	if (action.isInRange(handle))
	{
		executeStateNow(ai, AIState::PerformAction);
	}
	else
	{
		auto optionalTarget = action.trySetTarget(handle);
		if (optionalTarget.has_value())
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
			setNextState(ai, AIState::Think);
		}
	}
}

void drft::system::ArtificialInput::aiPerformAction(component::AI& ai) const
{
	if (!isPlanValid(ai.blackboard, ai.plan, getCurrentGoal(ai)))
	{
		executeStateNow(ai, AIState::Think);
		return;
	}

	auto handle = getHandle(ai);
	const auto& action = goap::ActionRegistry::get(ai.plan.front());
	if (!action.isInRange(handle))
	{
		executeStateNow(ai, AIState::MoveTo);
	}
	else
	{
		const auto result = action.perform(getHandle(ai));
		switch (result)
		{
		case goap::ActionResult::Complete:
			ai.plan.pop_front();
			setNextState(ai, AIState::Think);
			break;
		case goap::ActionResult::Continue:
			break;
		case goap::ActionResult::Error:
		case goap::ActionResult::Failed:
		default:
			ai.target = entt::null;
			ai.plan.clear();
			setNextState(ai, AIState::Think);
			break;
		}
	}
}
