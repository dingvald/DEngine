#include "pch.h"
#include "ToggleSprintAbility.h"
#include "Components/StaminaComponent.h"
#include "Components/SprintingComponent.h"

#include <Defines/CommonGuiColors.h>

#include <Systems/Gameplay/SkillsSystem.h>
#include <Utility/Math.h>
#include <Utility/TGUIHelpers.h>


drft::AbilityTargetingType drft::ToggleSprintAbility::getTargetingType() const
{
    return AbilityTargetingType::Auto;
}

bool drft::ToggleSprintAbility::isValid(entt::const_handle actor) const
{
    if (auto stamina = actor.try_get<StaminaComponent>())
    {
        if (stamina->current > 0) return true;
    }
    return false;
}

void drft::ToggleSprintAbility::perform(entt::handle actor, std::optional<TilePosition> targetPosition) const
{
	if (actor.all_of<SprintingComponent>())
	{
		actor.remove<SprintingComponent>();
	}
	else
	{
		const float speedMultiplier = calculateSpeedMultiplier(actor);
		const float staminaCost = calculateStaminaCost(actor);
		actor.emplace<SprintingComponent>(speedMultiplier, staminaCost);
	}
}

int drft::ToggleSprintAbility::getCost() const
{
	return 0;
}

bool drft::ToggleSprintAbility::isToggledOn(entt::const_handle actor) const
{
	if (actor.all_of<SprintingComponent>())
	{
		return true;
	}
	return false;
}

drft::AbilityIconData drft::ToggleSprintAbility::getIconData() const
{
	return AbilityIconData{
		.textureId = entt::hashed_string{"icons"},
		.color = guiColor::StaminaGreen,
		.uv = {1,0},
	};
}

entt::id_type drft::ToggleSprintAbility::getAssociatedSkill() const
{
	return SkillId::Agility;
}

float drft::ToggleSprintAbility::calculateSpeedMultiplier(entt::const_handle actor) const
{
	const int agility = system::SkillsSystem::getSkillLevel(SkillId::Agility, actor);
	float result = 1.5f;
	result += (logf(agility) / logf(3.9));
	return math::floorToMultiple(result, 0.5f);
}

float drft::ToggleSprintAbility::calculateStaminaCost(entt::const_handle actor) const
{
	return 2.0f;
}

std::string drft::ToggleSprintAbility::getContextualDescription(entt::const_handle actor) const
{
	const float speedMultiplier = calculateSpeedMultiplier(actor);
	const float staminaCost = calculateStaminaCost(actor);

	return std::format(
        "Increases speed by x{}. Consume {} extra stamina while moving.",
		GuiHelpers::colorizedString(std::format("{:.1f}", speedMultiplier), sf::Color::Yellow),
		GuiHelpers::colorizedString(std::format("{:.1f}", staminaCost), sf::Color::Yellow));
}
