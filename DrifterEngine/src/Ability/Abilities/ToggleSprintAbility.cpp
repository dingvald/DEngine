#include "pch.h"
#include "ToggleSprintAbility.h"
#include "Components/StaminaComponent.h"
#include "Components/SprintingComponent.h"

#include <Defines/CommonGuiColors.h>


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
		actor.emplace<SprintingComponent>();
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
