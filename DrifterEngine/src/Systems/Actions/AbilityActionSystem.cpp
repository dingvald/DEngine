#include "pch.h"
#include "AbilityActionSystem.h"

#include <Ability/AbilityRegistry.h>
#include <Ability/AbilityHelpers.h>

#include <Components/Actions/AbilityAction.h>
#include <Components/PlayerInputComponent.h>
#include <Components/CurrentActorComponent.h>

#include <Systems/Core/ActorSystem.h>



void drft::system::AbilityActionSystem::init()
{
	AbilityRegistry::bind();
}

void drft::system::AbilityActionSystem::update()
{
	auto playerView = _registry.view<AbilityAction, PlayerInputComponent, CurrentActorComponent>();
	for (auto&& [entity, action, player, currentActor] : playerView.each())
	{
		if (currentActor.state == CurrentActorState::InProgress) continue;

		const IAbility& ability = AbilityRegistry::get(action.ability);
		entt::handle handle = { _registry, entity };
		if (ability.isValid(handle))
		{
			switch (ability.getTargetingType())
			{
			case AbilityTargetingType::Self:
			case AbilityTargetingType::Auto:
				AbilityHelpers::performAbility(handle, ability, std::nullopt);
				break;
			case AbilityTargetingType::SelectDirection:
				AbilityHelpers::performDirectionalAbility(handle, ability);
				break;
			case AbilityTargetingType::SelectSquare:
				AbilityHelpers::performTargetedAbility(handle, ability);
				break;
			default:
				throw std::exception("Unhandled targetting type for ability.");
				break;
			}
		}

		handle.remove<AbilityAction>();
	}

	auto nonPlayerView = _registry.view<AbilityAction, CurrentActorComponent>(entt::exclude<PlayerInputComponent>);
	for (auto&& [entity, action, currentActor] : nonPlayerView.each())
	{
		if (currentActor.state == CurrentActorState::InProgress) continue;

		const auto& ability = AbilityRegistry::get(action.ability);
		entt::handle handle = { _registry, entity };
		if (ability.isValid(handle))
		{
			ability.perform(handle, action.target);
			ActorSystem::setActionComplete(handle, ActionCategory::Act, ability.getTimeCost());
		}

		handle.remove<AbilityAction>();
	}
}
