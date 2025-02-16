#include "pch.h"
#include "AbilityActionSystem.h"

#include <Ability/AbilityRegistry.h>

#include <Components/Actions/AbilityAction.h>
#include <Components/Components.h>
#include <Components/PlayerInputComponent.h>
#include <Components/PositionComponent.h>

#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>

#include <Systems/Core/ActorSystem.h>



void drft::system::AbilityActionSystem::init()
{
	AbilityRegistry::bind();
}

void drft::system::AbilityActionSystem::update()
{
	auto playerView = _registry.view<AbilityAction, PlayerInputComponent>();
	for (auto&& [entity, action, player] : playerView.each())
	{
		const auto& ability = AbilityRegistry::get(action.ability);
		entt::handle handle = { _registry, entity };
		if (ability.isValid(handle))
		{
			switch (ability.getTargetingType())
			{
			case AbilityTargetingType::Auto:
				ability.perform(handle);
				ActorSystem::setActionComplete(handle, ActionCategory::Act, ability.getCost());
				break;
			case AbilityTargetingType::SelectDirection:
			{
				auto tilePosition = handle.get<PositionComponent>().tile;
				handle.emplace<component::action::SelectDirection>(
					[tilePosition, &ability, &handle](sf::Vector2i direction) -> bool
					{
						ability.perform(handle, tilePosition + spatial::asTileSpace(direction));
						ActorSystem::setActionComplete(handle, ActionCategory::Act, ability.getCost());
						return true;
					});
			}
			break;
			case AbilityTargetingType::SelectSquare:
			{
				auto range = ability.getRange(handle);
				auto targetingShape = ability.getTargetingShape(handle);
				handle.emplace<component::action::SelectTarget>(range, targetingShape,
					[&ability, handle](sf::Vector3i position) -> bool {
						ability.perform(handle, spatial::asTileSpace(position));
						ActorSystem::setActionComplete(handle, ActionCategory::Act, ability.getCost());
						return true;
					});
			}
			break;
			default:
				throw std::exception("Unhandled targetting type for ability.");
				break;
			}
		}

		handle.remove<AbilityAction>();
	}

	auto nonPlayerView = _registry.view<AbilityAction>(entt::exclude<PlayerInputComponent>);
	for (auto&& [entity, action] : nonPlayerView.each())
	{
		const auto& ability = AbilityRegistry::get(action.ability);
		entt::handle handle = { _registry, entity };
		if (ability.isValid(handle))
		{
			ability.perform(handle, action.target);
			ActorSystem::setActionComplete(handle, ActionCategory::Act, ability.getCost());
		}

		handle.remove<AbilityAction>();
	}
}
