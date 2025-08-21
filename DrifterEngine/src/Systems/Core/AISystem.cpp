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

using namespace entt::literals;

void drft::system::AiSystem::init()
{
	_utility.loadUtilityArchetypes(STATIC_DATA_DIRECTORY / "utility_ai_archetypes");

	_inputClearingHouse.registerInput("my_health"_hs, AiInputFunctions::MyHealth);
	_inputClearingHouse.registerInput("target_health"_hs, AiInputFunctions::TargetHealth);
	_inputClearingHouse.registerInput("distance_to_target"_hs, AiInputFunctions::DistanceToTarget);
	_inputClearingHouse.registerInput("target_relationship"_hs, AiInputFunctions::TargetRelationship);
	_utility.setInputProvider(_inputClearingHouse);

	_actionRegistry.registerAction<MeleeAttackAiAction>("melee_attack"_hs);
	_actionRegistry.registerAction<WaitAiAction>("wait"_hs);
}

void drft::system::AiSystem::update()
{
	auto view = _registry.view<UtilityAIComponent, CurrentActorComponent>();
	for (auto&& [entity, ai, currentActor] : view.each())
	{
		auto scoredActions = _utility.scoreActions(entity, ai.archetype, ai.blackboard);

		entt::handle actor_handle = { _registry, entity };
		auto [action, target] = selectAction(scoredActions, actor_handle);
		if (!action) continue;

		entt::const_handle target_handle = { _registry, target };
		if (action->isInRange(actor_handle, target_handle))
		{
			action->perform(actor_handle, target_handle);
		}
		else
		{
			auto moveToAction = _actionRegistry.getAction("move_to"_hs);
			if (!moveToAction)
			{
				throw std::exception("No move to action in the registry, it must exist");
			}
			moveToAction->perform(actor_handle, target_handle);
		}
	}
}

std::pair<const IAiAction*, entt::entity> drft::system::AiSystem::selectAction(const UtilityAI<entt::entity>::ScoredActions& actions, entt::const_handle actor) const
{
	for (auto&& [score, actionTargetPair] : actions)
	{
		auto* action = _actionRegistry.getAction(actionTargetPair.action);
		if (!action) continue;
		if (!action->canPerform(actor)) continue;

		return std::make_pair(action, actionTargetPair.target);
	}

	// default to wait action
	auto* waitAction = _actionRegistry.getAction("wait"_hs);
	if (!waitAction)
	{
		throw std::exception("No wait action in the registry, it must exist");
	}

	return { waitAction, actor.entity() };
}
