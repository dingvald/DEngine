#include "pch.h"
#include "ThrowAbility.h"
#include "Spatial/Helpers.h"

#include "Components/BodyComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/MaterialComponent.h"

#include "Systems/Helpers/ItemDatabase.h"

drft::AbilityTargetingType drft::ThrowAbility::getTargetingType() const
{
	return AbilityTargetingType::SelectSquare;
}

bool drft::ThrowAbility::isValid(entt::const_handle actor) const
{
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped("held", 0);
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
		auto item = body->parts.getEquipped("held", 0);
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
	const int maxRange = 12;
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped("held", 0);
		if (item)
		{
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			if (auto material = actor.registry()->try_get<MaterialComponent>(itemEntity))
			{
				int rangeVal = std::max(1, (maxRange - static_cast<int>(material->weight)));
				return { 0, rangeVal };
			}
		}

	}
	return {0, 0};
}

std::vector<sf::Vector2i> drft::ThrowAbility::getTargetingShape(entt::const_handle actor) const
{
	return { {0,0} };
}

drft::AbilityIconData drft::ThrowAbility::getIconData() const
{
	return AbilityIconData{
		.textureId = entt::hashed_string{"icons"},
		.color = sf::Color::Magenta,
		.uv = {2,0},
		.uvSize = {16, 16}
	};
}
