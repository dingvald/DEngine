#include "pch.h"
#include "AISystem.h"

#include <Engine/CommonEngineDirectories.h>

#include <Components/CurrentActorComponent.h>
#include <Components/UtilityAIComponent.h>

#include <AI/InputClearingHouse/AIDataClearingHouse.h>
#include <AI/InputClearingHouse/InputFunctions.h>
#include <AI/AiActions/AiActionRegistry.h>

#include <AI/AiActions/MeleeAttackAiAction.h>

using namespace entt::literals;

class MockInputProvider : public IUtilityInputProvider<entt::entity>
{
	float getInput(UtilityInputID inputID, entt::entity actor, entt::entity target) const override
	{
		return 0.0f;
	}
};

void drft::system::AISystem::init()
{
	_utility.loadUtilityArchetypes(STATIC_DATA_DIRECTORY / "utility_ai_archetypes");

	_inputClearingHouse = std::make_unique<AIDataClearingHouse>(_registry);
	_inputClearingHouse->registerInput("my_health"_hs, AiInputFunctions::MyHealth);
	_inputClearingHouse->registerInput("target_health"_hs, AiInputFunctions::TargetHealth);
	_inputClearingHouse->registerInput("distance_to_target"_hs, AiInputFunctions::DistanceToTarget);
	_inputClearingHouse->registerInput("target_relationship"_hs, AiInputFunctions::TargetRelationship);
	_utility.setInputProvider(*_inputClearingHouse);

	_actionRegistry = std::make_unique<AiActionRegistry>();
	_actionRegistry->registerAction("melee_attack"_hs, std::make_unique<MeleeAttackAiAction>());
}

void drft::system::AISystem::update()
{
	auto view = _registry.view<UtilityAIComponent, CurrentActorComponent>();
	for (auto&& [entity, ai, currentActor] : view.each())
	{
		auto&& scoredActions = _utility.scoreActions(entity, ai.type, ai.blackboard);

		// Select action here

	}
}
