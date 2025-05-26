#include "pch.h"
#include "ThrowAbility.h"
#include "Spatial/Helpers.h"

#include "Components/BodyComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/WeightComponent.h"
#include <Components/DominantSideComponent.h>

#include <Defines/CommonGuiColors.h>

#include "Systems/Helpers/ItemDatabase.h"
#include <Utility/EntityAccessors/GetEntityName.h>
#include <Systems/Helpers/GetDominantSide.h>
#include <Systems/Gameplay/SkillsSystem.h>
#include <Utility/TGUIHelpers.h>

namespace Internal
{
	static int calculateMaxRange(int strength, float weight)
	{
		if (weight <= FLT_EPSILON) return 1;

		float result = 1.0f;
		result += 0.6f * (logf(static_cast<float>(strength)) / logf(1.1f));

		return static_cast<int>(result / std::powf(weight, 1.5f));
	}
}

drft::AbilityTargetingType drft::ThrowAbility::getTargetingType() const
{
	return AbilityTargetingType::SelectSquare;
}

bool drft::ThrowAbility::isValid(entt::const_handle actor) const
{
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(actor));
		return item != 0u;
	}
	return false;
}

void drft::ThrowAbility::perform(entt::handle actor, std::optional<TilePosition> targetPosition) const
{
	const float throwSpeed = 6.0f;
	if (!targetPosition.has_value()) throw std::exception("You need a target to throw at.");
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(actor));
		if (item)
		{
			body->parts.removeItem(item);
			auto& throwerPos = actor.get<PositionComponent>();
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);

			if (targetPosition.value() == throwerPos.tile)
			{
				actor.registry()->emplace<PositionComponent>(itemEntity, targetPosition.value());
			}
			else
			{
				auto line = spatial::getLine2d(spatial::toXY(throwerPos.tile), spatial::toXY(targetPosition.value()));

				TilePosition projectilePosition = { line.front().x, line.front().y, throwerPos.tile.z };
				actor.registry()->emplace<PositionComponent>(itemEntity, projectilePosition);
				actor.registry()->emplace<ProjectileComponent>(itemEntity, std::move(line), 1, throwSpeed);
			}
		}
	}
}

drft::math::Range<int> drft::ThrowAbility::getRange(entt::const_handle actor) const
{	
	int strength = system::SkillsSystem::getSkillLevel(SkillId::Strength, actor);
	float weight = 0.f;
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(actor));
		if (item)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			if (auto weightComponent = actor.registry()->try_get<WeightComponent>(itemEntity))
			{
				weight = weightComponent->value;
			}
		}
	}

	return {0, Internal::calculateMaxRange(strength, weight)};
}

std::vector<sf::Vector2i> drft::ThrowAbility::getTargetingShape(entt::const_handle actor) const
{
	return { {0,0} };
}

drft::AbilityIconData drft::ThrowAbility::getIconData() const
{
	return AbilityIconData{
		.textureId = entt::hashed_string{"icons"},
		.color = guiColor::MGSHudBlue,
		.uv = {2,0},
	};
}

entt::id_type drft::ThrowAbility::getAssociatedSkill() const
{
	return SkillId::Strength;
}

std::string drft::ThrowAbility::getContextualDescription(entt::const_handle actor) const
{
	std::string dominantHandString = "dominant hand";
	std::string range = std::to_string(0);

	if (auto dominantSide = actor.try_get<DominantSideComponent>())
	{
		dominantHandString = std::format("{} hand", dominantSide->side);
	}

	range = std::to_string(getRange(actor).getMax());

	return std::format(
		"Throws the item in your {} a maximum of {} meters.\nThe range depends on the item's weight.", 
		dominantHandString, 
		GuiHelpers::colorizedString(range, guiColor::VariableGreen));
}
