#include "pch.h"
#include "ArtificialInput.h"

#include "Components/Actions/MoveAction.h"
#include "Components/AIComponent.h"
#include "Components/PositionComponent.h"
#include "Components/FactionComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/Tags.h"

#include "Events/TurnEndEvent.h"
#include "Random/RandomNumberGenerator.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Systems/Gameplay/FactionSystem.h"
#include "Systems/Helpers/HasLineOfSight.h"
#include "GOAP/Plan.h"
#include "GOAP/GoalRegistry.h"
#include "GOAP/ActionRegistry.h"
#include "GOAP/WorldStateTypes.h"
#include "GOAP/Sensors/HostileSensor.h"


void drft::system::ArtificialInput::init()
{
	goap::ActionRegistry::bind();
	goap::GoalRegistry::bind();

	_sensorySystem.registerSensor(std::make_unique<goap::HostileSensor>());
	_sensorySystem.registerChecker(hasLineOfSight, goap::SensorType::Visual);

	auto& dispatcher = _registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::TurnEndEvent>().connect<&ArtificialInput::onTurnEndEvent>(this);
}

void drft::system::ArtificialInput::update(const float dt)
{
	auto view = _registry->view<AIComponent, const PositionComponent, component::tag::CurrentActor>();
	for (auto [entity, ai, myPos] : view.each())
	{
		senseWorldState(ai);
		executeStateNow(ai, ai.state);
	}
}

bool drft::system::ArtificialInput::inSightRange(sf::Vector2i position, const AIComponent& ai) const
{
	auto myPosition = _registry->get<PositionComponent>(entt::to_entity(*_registry, ai)).position;
	if (spatial::distance(myPosition, position) < ai.sightRange) return true;
	return false;
}

void drft::system::ArtificialInput::moveToTarget(entt::handle entity, sf::Vector2i targetPosition) const
{
	auto& position = entity.get<PositionComponent>().position;
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

	entity.emplace_or_replace<PerformMoveAction>(sf::Vector2i{ xMove, yMove });
}

void drft::system::ArtificialInput::pathToTarget(entt::handle entity, sf::Vector2i targetPosition) const
{
	auto& position = entity.get<PositionComponent>().position;
	if (!_cachedPaths.contains(entity.entity()) || _cachedPaths.at(entity.entity()).empty())
	{
		const auto& grid = _registry->ctx().get<const spatial::WorldGrid&>();
		_cachedPaths[entity.entity()] = grid.getPath(position, targetPosition,
			[this](const std::vector<entt::entity>& entities) -> int
			{
				for (auto entity : entities)
				{
					if (auto material = _registry->try_get<MaterialComponent>(entity))
					{
						if (material->blocks) return 1000;
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

entt::handle drft::system::ArtificialInput::getHandle(const AIComponent& ai) const
{
	entt::entity entity = entt::to_entity(*_registry, ai);
	entt::handle aiHandle = { *_registry, entity };
	return aiHandle;
}

void drft::system::ArtificialInput::onTurnEndEvent(const events::TurnEndEvent& ev)
{
	if (auto ai = _registry->try_get<AIComponent>(ev.entity))
	{
		if (_sensorySystem.decayMemory(ai->surroundings))
		{
			// HACKZZ: May still want to preserve some things on the blackboard.
			ai->blackboard.clear();
		}
	}
}

void drft::system::ArtificialInput::senseWorldState(AIComponent& ai)
{
	_sensorySystem.runSensors(getHandle(ai));
}

std::deque<drft::goap::AiAction> drft::system::ArtificialInput::generatePlan(AIComponent& ai, std::deque<GoalName>& goals) const
{
	while (!goals.empty())
	{
		const auto& goal = goap::GoalRegistry::get(goals.front());
		if (ai.blackboard.contains(goal.desiredState()))
		{
			goals.pop_front();
			continue;
		}
		auto optionalPlan = goap::plan(ai.blackboard, getAiActions(ai), goal.desiredState());
		ai.currentGoal = goals.front();
		goals.pop_front();
		if (optionalPlan.has_value())
		{
			return optionalPlan.value();
		}
	}
	// Fallback
	ai.currentGoal = {};
	return std::deque<goap::AiAction>{goap::AiAction::RandomMove};
}

bool drft::system::ArtificialInput::isPlanValid(const goap::WorldState& worldState, const goap::Plan& plan) const
{
	if (plan.empty()) return false;
	const auto& firstAction = goap::ActionRegistry::get(plan.front());
	return worldState.contains(firstAction.preconditions());
}

std::deque<drft::system::ArtificialInput::GoalName> drft::system::ArtificialInput::prioritizeGoals(const AIComponent& ai) const
{
	const float THRESHOLD = 0.15f;
	std::deque<std::string> result;
	std::deque<std::string> topContenders;
	std::deque<std::string> others;

	std::map<float, std::string, std::greater<float>> ranking;
	float maxUtility = 0.0f;
	for (auto&& goalName : ai.goals)
	{
		const auto& goal = goap::GoalRegistry::get(goalName);
		float utility = goal.utility(getHandle(ai));
		maxUtility = std::max(maxUtility, utility);
		ranking.emplace(utility, goalName);
	}
	
	for (auto&& [util, goalName] : ranking)
	{
		if (util >= maxUtility - THRESHOLD)
		{
			topContenders.push_back(goalName);
		}
		else
		{
			others.push_back(goalName);
		}
	}
	auto rd = std::random_device{};
	auto rng = std::default_random_engine{ rd() };
	std::shuffle(topContenders.begin(), topContenders.end(), rng);
	for (int i = 0; i < topContenders.size(); ++i)
	{
		if (topContenders[i] == ai.currentGoal)
		{
			std::swap(topContenders[i], topContenders[0]);
		}
	}

	result.insert(result.end(), topContenders.begin(), topContenders.end());
	result.insert(result.end(), others.begin(), others.end());

	return result;
}

std::unordered_set<drft::goap::AiAction> drft::system::ArtificialInput::getAiActions(const AIComponent& ai) const
{
	std::unordered_set<goap::AiAction> result;
	entt::const_handle entity = { *_registry, entt::to_entity(*_registry, ai) };
	result.insert(goap::AiAction::RandomMove);

	if (entity.all_of<FactionComponent>())
	{
		result.insert(goap::AiAction::SpotHostile);
		result.insert(goap::AiAction::AttackHostile);
		result.insert(goap::AiAction::EscapeHostiles);
		result.insert(goap::AiAction::InvestigateHostile);
	}

	return result;
}

void drft::system::ArtificialInput::setNextState(AIComponent& ai, AIState state) const
{
	ai.state = state;
}

void drft::system::ArtificialInput::executeStateNow(AIComponent& ai, AIState state) const
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

void drft::system::ArtificialInput::aiThink(AIComponent& ai) const
{
	auto goals = prioritizeGoals(ai);
	if (goals.front() != ai.currentGoal
		|| !isPlanValid(ai.blackboard, ai.plan))
	{
		ai.plan = generatePlan(ai, goals);
	}
	executeStateNow(ai, AIState::MoveTo);
}

void drft::system::ArtificialInput::aiMoveTo(AIComponent& ai) const
{
	if (!isPlanValid(ai.blackboard, ai.plan))
	{
		executeStateNow(ai, AIState::Think);
		return;
	}

	auto handle = getHandle(ai);
	const goap::IAction& action = goap::ActionRegistry::get(ai.plan.front());

	if (action.isInRange(handle))
	{
		clearPathCache(handle.entity());
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
			// replan required
			clearPathCache(handle.entity());
			setNextState(ai, AIState::Think);
		}
	}
}

void drft::system::ArtificialInput::aiPerformAction(AIComponent& ai) const
{
	if (!isPlanValid(ai.blackboard, ai.plan))
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
			ai.blackboard[goap::action_counter] = 0;
			break;
		case goap::ActionResult::Continue:
			ai.blackboard[goap::action_counter]++;
			break;
		case goap::ActionResult::Error:
		case goap::ActionResult::Failed:
		default:
			ai.blackboard[goap::action_counter] = 0;
			ai.target = entt::null;
			ai.plan.clear();
			break;
		}
		setNextState(ai, AIState::Think);
	}
}
