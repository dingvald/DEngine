#include "pch.h"
#include "AISystem.h"

#include <Engine/CommonEngineDirectories.h>

#include <Components/CurrentActorComponent.h>
#include <Components/UtilityAIComponent.h>

#include <AI/InputClearingHouse/AIDataClearingHouse.h>
#include <AI/InputClearingHouse/InputFunctions.h>

#include <AI/AiActions/AiActionRegistry.h>
#include <AI/AiActions/IAiAction.h>
#include <AI/AiActions/MeleeAttackAiAction.h>
#include <AI/AiActions/WaitAiAction.h>
#include <AI/AiActions/MoveToAiAction.h>

#include <AI/Sensor/Sensors/VisualActorSensor.h>

#include <Utility/StandardLogger.h>

#pragma optimize("", off)

using namespace entt::literals;

void drft::system::AiSystem::init()
{
	_utility.loadUtilityArchetypes(STATIC_DATA_DIRECTORY / "utility_ai_archetypes");
	_utility.setInputProvider(_inputClearingHouse);

	// Register sensors
	_sensors.registerSensor<VisualActorSensor>();

	// Register AI consideration inputs
	_inputClearingHouse.registerInput("my_health"_hs, AiInputFunctions::MyHealth);
	_inputClearingHouse.registerInput("target_health"_hs, AiInputFunctions::TargetHealth);
	_inputClearingHouse.registerInput("distance_to_target"_hs, AiInputFunctions::DistanceToTarget);
	_inputClearingHouse.registerInput("target_relationship"_hs, AiInputFunctions::TargetRelationship);

	// Register actions
	_actionRegistry.registerAction<MeleeAttackAiAction>("melee_attack"_hs);
	_actionRegistry.registerAction<WaitAiAction>("wait"_hs);
	_actionRegistry.registerAction<MoveToAiAction>("move_to"_hs);

	// Set default actions
	setMoveToAction(_actionRegistry.getAction("move_to"_hs));
	setDefaultAction(_actionRegistry.getAction("wait"_hs));
}

void drft::system::AiSystem::update()
{
	auto view = _registry.view<UtilityAIComponent, CurrentActorComponent>();
	for (auto&& [entity, ai, currentActor] : view.each())
	{
		entt::handle actor = { _registry, entity };

		_sensors.runSensors(actor);

		auto&& [action, target] = selectAction(actor, ai);
		action->perform(actor, { _registry, target });
	}
}

void drft::system::AiSystem::setDefaultAction(const IAiAction* defaultAction)
{
	if (!defaultAction)
	{
		LOG_ERROR("Setting default action to nullptr");
	}
	_defaultAction = defaultAction;
}

void drft::system::AiSystem::setMoveToAction(const IAiAction* moveToAction)
{
	if (!moveToAction)
	{
		LOG_ERROR("Setting moveTo action to nullptr");
	}
	_moveToAction = moveToAction;
}

std::pair<const IAiAction*, entt::entity> drft::system::AiSystem::selectAction(entt::const_handle actor, const UtilityAIComponent& ai) const
{
	auto scoredActions = _utility.scoreActions(actor.entity(), ai.archetype, ai.blackboard);

	auto result = std::make_pair(_defaultAction, actor.entity());

	for (auto&& [score, actionTargetPair] : scoredActions)
	{
		if (score <= 0.f) continue;

		auto* action = _actionRegistry.getAction(actionTargetPair.action);
		if (!action) continue;
		if (!action->canPerform(actor)) continue;

		result = std::make_pair(action, actionTargetPair.target);
		break;
	}

	if (!result.first->isInRange(actor, { *actor.registry(), result.second }))
	{
		result.first = _moveToAction;
	}

	return result;
}
