#include "pch.h"
#include "AISystem.h"

#include <Engine/CommonEngineDirectories.h>

#include <Components/CurrentActorComponent.h>
#include <Components/PathNavComponent.h>
#include <Components/PositionComponent.h>
#include <Components/UtilityAIComponent.h>

#include <AI/InputClearingHouse/AIDataClearingHouse.h>
#include <AI/InputClearingHouse/InputFunctions.h>

#include <AI/AiActions/AiActionInstance.h>
#include <AI/AiActions/AiActionRegistry.h>
#include <AI/AiActions/IAiAction.h>
#include <AI/AiActions/MeleeAttackAiAction.h>
#include <AI/AiActions/WaitAiAction.h>
#include <AI/AiActions/MoveToAiAction.h>

#include <AI/Sensor/Sensors/VisualActorSensor.h>

#pragma optimize("", off)

static const float UTILITY_SCORE_DECAY = 0.25f;

using namespace entt::literals;

void drft::system::AiSystem::init()
{
	_utility.loadUtilityArchetypes(STATIC_DATA_DIRECTORY / "utility_ai_archetypes");
	_utility.setInputProvider(_inputClearingHouse);

	// Register sensors
	_sensors.registerSensor<VisualActorSensor>();

	// Register AI consideration inputs
	_inputClearingHouse.registerInput("my_health"_hs, AiInputFunctions::myHealth);
	_inputClearingHouse.registerInput("target_health"_hs, AiInputFunctions::targetHealth);
	_inputClearingHouse.registerInput("distance_to_target"_hs, AiInputFunctions::distanceToTarget);
	_inputClearingHouse.registerInput("target_relationship"_hs, AiInputFunctions::targetRelationship);

	// Register actions
	_actionRegistry.registerAction<MeleeAttackAiAction>("melee_attack"_hs);
	_actionRegistry.registerAction<WaitAiAction>("wait"_hs);
	_actionRegistry.registerAction<MoveToAiAction>("move_to"_hs);

	// Set default actions
	setMoveToAction(_actionRegistry.getAction("move_to"_hs));
	setDefaultAction(_actionRegistry.getAction("wait"_hs));
}

void drft::system::AiSystem::update(const float dt)
{
	auto view = _registry.view<UtilityAIComponent, CurrentActorComponent>();
	for (auto&& [entity, ai, currentActor] : view.each())
	{
		if (currentActor.state != CurrentActorState::Pending) continue;

		entt::handle actor = { _registry, entity };

		ai.blackboard.clear();
		_sensors.runSensors(actor, ai.blackboard);

		AiActionInstance action = selectAction(actor, ai);
		ai.currentAction = action;
		action.perform(actor);
	}
}

void drft::system::AiSystem::setDefaultAction(const IAiAction* defaultAction)
{
	DEBUG_ASSERT(defaultAction);
	_defaultAction = defaultAction;
}

void drft::system::AiSystem::setMoveToAction(const IAiAction* moveToAction)
{
	DEBUG_ASSERT(moveToAction);
	_moveToAction = moveToAction;
}

AiActionInstance drft::system::AiSystem::selectAction(entt::handle actor, const UtilityAIComponent& ai) const
{
	// Previous action should be sticky, but should decay
	AiActionInstance result = ai.currentAction;
	result.score *= UTILITY_SCORE_DECAY;

	if (!result.isInRange(actor) || !result.isValid(actor) || result.score < 0.1f)
	{
		result.reset(_defaultAction);
	}

	auto scoredActions = _utility.scoreActions(actor.entity(), ai.archetype, ai.blackboard);
	for (auto&& [actionTargetPair, score] : scoredActions)
	{
		if (score <= 0.f) continue;
		if (score <= result.score) continue;

		const IAiAction* action = _actionRegistry.getAction(actionTargetPair.action);
		if (!action) continue;

		auto* position = _registry.try_get<PositionComponent>(actionTargetPair.target);
		std::optional<TilePosition> targetPosition;
		if (position) targetPosition.emplace(position->tile);

		if (!action->isValid(actor, targetPosition)) continue;

		result.action = action;
		result.score = score;
		result.target = targetPosition;
	}

	if (result.isInRange(actor))
	{
		actor.remove<PathNavComponent>();
	}
	else
	{
		result.action = _moveToAction;
	}

	return result;
}
