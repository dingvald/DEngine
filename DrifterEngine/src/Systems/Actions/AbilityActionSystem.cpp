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
				performAutoAbility(handle, ability);
				break;
			case AbilityTargetingType::SelectDirection:
				performDirectionalAbility(handle, ability);
				break;
			case AbilityTargetingType::SelectSquare:
				performTargetedAbility(handle, ability);
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

void drft::system::AbilityActionSystem::performAutoAbility(entt::handle actor, const IAbility& ability) const
{
	ability.perform(actor);
	ActorSystem::setActionComplete(actor, ActionCategory::Act, ability.getCost());
}

void drft::system::AbilityActionSystem::performTargetedAbility(entt::handle actor, const IAbility& ability) const
{
	auto range = ability.getRange(actor);
	auto targetingShape = ability.getTargetingShape(actor);
	actor.emplace<component::action::SelectTarget>(range, targetingShape,
		[&ability, actor](sf::Vector3i position) -> bool 
		{
			ability.perform(actor, spatial::asTileSpace(position));
			ActorSystem::setActionComplete(actor, ActionCategory::Act, ability.getCost());
			return true;
		});
}

void drft::system::AbilityActionSystem::performDirectionalAbility(entt::handle actor, const IAbility& ability) const
{
	auto tilePosition = actor.get<PositionComponent>().tile;
	actor.emplace<component::action::SelectDirection>(
		[tilePosition, &ability, actor](sf::Vector2i direction) -> bool
		{
			ability.perform(actor, tilePosition + spatial::asTileSpace(direction));
			ActorSystem::setActionComplete(actor, ActionCategory::Act, ability.getCost());
			return true;
		});
}
