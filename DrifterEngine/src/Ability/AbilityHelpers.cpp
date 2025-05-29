#include "pch.h"
#include "AbilityHelpers.h"
#include <Ability/IAbility.h>
#include <Systems/Core/ActorSystem.h>
#include <Components/Components.h>
#include <Spatial/Conversions.h>
#include <Components/PositionComponent.h>
#include <Components/HealthComponent.h>
#include <Components/StaminaComponent.h>

#include <Utility/StandardLogger.h>

void AbilityHelpers::performAbility(entt::handle actor, const drft::IAbility& ability, std::optional<drft::TilePosition> target)
{
	ability.perform(actor, target);
	drft::system::ActorSystem::setActionComplete(actor, drft::system::ActionCategory::Act, ability.getTimeCost());
}

void AbilityHelpers::performTargetedAbility(entt::handle actor, const drft::IAbility& ability)
{
	auto range = ability.getRange(actor);
	auto targetingShape = ability.getTargetingShape(actor);
	actor.emplace<component::action::SelectTarget>(range, targetingShape,
		[&ability, actor](sf::Vector3i position) -> bool
		{
			performAbility(actor, ability, drft::spatial::asTileSpace(position));
			return true;
		});
}

void AbilityHelpers::performDirectionalAbility(entt::handle actor, const drft::IAbility& ability)
{
	drft::TilePosition tilePosition = actor.get<PositionComponent>().tile;
	actor.emplace<component::action::SelectDirection>(
		[tilePosition, &ability, actor](sf::Vector2i direction) -> bool
		{
			drft::TilePosition target = tilePosition + drft::spatial::asTileSpace(direction);
			performAbility(actor, ability, target);
			return true;
		});
}

bool AbilityHelpers::hasResources(entt::const_handle actor, const drft::IAbility& ability)
{
	for (auto&& [resource, amount] : ability.getResourceCosts(actor))
	{
		switch (resource)
		{
		case drft::AbilityResourceType::Health:
			if (auto health = actor.try_get<HealthComponent>())
			{
				if (health->current >= amount) continue;
			}
			return false;
		case drft::AbilityResourceType::Stamina:
			if (auto stamina = actor.try_get<StaminaComponent>())
			{
				if (stamina->current >= amount) continue;
			}
			return false;
		default:
			LOG_ERROR("Unhandled enum");
			__debugbreak();
			break;
		}
	}
	return true;
}

void AbilityHelpers::spendResources(entt::handle actor, const drft::IAbility& ability)
{
	for (auto&& [resource, amount] : ability.getResourceCosts(actor))
	{
		switch (resource)
		{
		case drft::AbilityResourceType::Health:
			actor.emplace_or_replace<component::action::TakeDamage>(amount);
			break;
		case drft::AbilityResourceType::Stamina:
			actor.emplace_or_replace<component::action::ConsumeStamina>(amount);
			break;
		default:
			LOG_ERROR("Unhandled enum");
			__debugbreak();
			break;
		}
	}
}
