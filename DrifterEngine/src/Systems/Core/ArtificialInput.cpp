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

	registerSensor(std::make_unique<goap::HostileSensor>());
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

void drft::system::ArtificialInput::registerSensor(std::unique_ptr<goap::ISensor> sensor)
{
	_sensors.emplace_back(std::move(sensor));
}

void drft::system::ArtificialInput::senseWorldState(component::AI& ai) const
{
	auto handle = getHandle(ai);
	auto pos = handle.get<component::Position>();
	auto senseRadius = spatial::getIntCircleInRadius(pos.position, ai.sightRange);
	const auto& grid = handle.registry()->ctx().get<const spatial::WorldGrid&>();
	std::vector<entt::entity> surroundings;
	for (auto&& position : senseRadius)
	{
		auto entities = grid.entitiesAt(position);
		surroundings.insert(surroundings.end(), entities.begin(), entities.end());
	}

	for (auto&& sensor : _sensors)
	{
		ai.blackboard.merge(sensor->sense(handle, surroundings));
	}
}

std::deque<drft::goap::AiAction> drft::system::ArtificialInput::generatePlan(const component::AI& ai, const goap::Goal& exclude) const
{
	auto goals = prioritizeGoals(ai);
	if (!ai.plan.empty())
	{
		const auto& lastAction = goap::ActionRegistry::get(ai.plan.back());
		if (lastAction.effects().contains(goals.front())) return ai.plan;
	}
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

int drft::system::ArtificialInput::calculateGoalValue(const goap::Goal& goal, const component::AI& ai) const
{
	// TODO: This is just placeholder

	if (goal.contains(goap::WorldState{ {"look_busy", true} }))
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


// AI State Machine

void drft::system::ArtificialInput::aiThink(component::AI& ai)
{
	senseWorldState(ai);
	ai.plan = generatePlan(ai);
	ai.state = AIState::MoveTo;
}

void drft::system::ArtificialInput::aiMoveTo(component::AI& ai)
{
	auto handle = getHandle(ai);
	const auto& action = goap::ActionRegistry::get(ai.plan.front());

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
	auto handle = getHandle(ai);

	const auto& action = goap::ActionRegistry::get(ai.plan.front());
	if (action.requiresInRange() && !action.isInRange(handle))
	{
		ai.state = AIState::MoveTo;
	}
	else
	{
		const auto result = action.perform(getHandle(ai));
		switch (result)
		{
		case drft::goap::ActionResult::Complete:
			ai.plan.pop_front();
			ai.state = AIState::Think;
			break;
		case drft::goap::ActionResult::Continue:
			break;
		case drft::goap::ActionResult::Error:
		case drft::goap::ActionResult::Failed:
			ai.plan = generatePlan(ai, goap::ActionRegistry::get(ai.plan.back()).effects());
			ai.target = entt::null;
			ai.state = AIState::Think;
			break;
		default:
			ai.plan.pop_front();
			ai.target = entt::null;
			ai.state = AIState::Think;
			break;
		}
	}
}
