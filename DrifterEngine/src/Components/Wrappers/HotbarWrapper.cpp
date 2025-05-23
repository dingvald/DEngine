#include "pch.h"
#include "HotbarWrapper.h"
#include <Systems/Core/HUD.h>
#include <Systems/Helpers/ToHotbarIndex.h>
#include <Utility/StandardLogger.h>

void drft::HotbarWrapper::setAbility(entt::id_type abilityId, unsigned int hotbarIndex)
{
	if (!isValid()) return;

	size_t arrayIndex = system::fromHotbarIndex(hotbarIndex);
	if (arrayIndex > HotbarComponent::MAX_SIZE)
	{
		LOG_ERROR("Trying to set ability to invalid hotbar index {}", hotbarIndex);
		return;
	}

	if (auto hotbar = tryGetUnderlyingConst())
	{
		for (unsigned int i = 0; i < HotbarComponent::MAX_SIZE; i++)
		{
			if (hotbar->abilities.at(i) == abilityId)
			{
				removeAbility(system::toHotbarIndex(i));
			}
			if (i == arrayIndex)
			{
				modify([arrayIndex, abilityId](HotbarComponent& comp) { comp.abilities.at(arrayIndex) = abilityId; });
				system::HUD::refresh(*tryGetRegistry()); // Should not be null due to isValid check above
			}
		}
	}
}

entt::id_type drft::HotbarWrapper::getAbility(unsigned int hotbarIndex) const
{
	size_t arrayIndex = system::fromHotbarIndex(hotbarIndex);
	if (arrayIndex > HotbarComponent::MAX_SIZE)
	{
		LOG_ERROR("Trying to set ability to invalid hotbar index {}", hotbarIndex);
		return AbilityIds::NullAbility;
	}
	if (auto hotbar = tryGetUnderlyingConst())
	{
		return hotbar->abilities.at(arrayIndex);
	}
	return AbilityIds::NullAbility;
}

void drft::HotbarWrapper::removeAbility(unsigned int hotbarIndex)
{
	if (!isValid()) return;

	size_t arrayIndex = system::fromHotbarIndex(hotbarIndex);
	if (arrayIndex > HotbarComponent::MAX_SIZE)
	{
		LOG_ERROR("Trying to set ability to invalid hotbar index {}", hotbarIndex);
		return;
	}

	modify([arrayIndex](HotbarComponent& comp) { comp.abilities.at(arrayIndex) = AbilityIds::NullAbility; });
	system::HUD::refresh(*tryGetRegistry()); // Should not be null due to isValid check above
}

std::optional<unsigned int> drft::HotbarWrapper::findAbilityIndex(entt::id_type abilityId) const
{
	if (!isValid()) return std::nullopt;

	if (auto hotbar = tryGetUnderlyingConst())
	{
		for (unsigned int i = 0; i < HotbarComponent::MAX_SIZE; i++)
		{
			if (hotbar->abilities.at(i) == abilityId)
			{
				return system::toHotbarIndex(i);
			}
		}
	}
	return std::nullopt;
}
