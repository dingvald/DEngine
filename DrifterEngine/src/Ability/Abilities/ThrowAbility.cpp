#include "pch.h"
#include "ThrowAbility.h"
#include "Spatial/Helpers.h"

#include "Components/BodyComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/MaterialComponent.h"

#include "Systems/Helpers/ItemDatabase.h"
#include "Spatial/Helpers.h"

sf::Color drft::ThrowAbility::getIconColor() const
{
	return sf::Color::Magenta;
}

sf::IntRect drft::ThrowAbility::getTextureUV() const
{
	return { 2, 0, 16, 16 };
}

drft::AbilityTargetingType drft::ThrowAbility::getTargetingType() const
{
	return AbilityTargetingType::SelectSquare;
}

bool drft::ThrowAbility::isValid(entt::const_handle actor) const
{
	if (auto body = actor.try_get<BodyComponent>())
	{
		if (const auto rightHand = body->parts.search("Right Hand"))
		{
			auto optionalItem = rightHand->getSlotItem(EquipmentLayer::Held);
			if (optionalItem.has_value()) return true;
		}
	}
	return false;
}

void drft::ThrowAbility::perform(entt::handle actor, std::optional<TilePosition> targetPosition) const
{
	const float throwSpeed = 6.0f;
	if (!targetPosition.has_value()) throw std::exception("You need a target to throw at.");
	if (auto body = actor.try_get<BodyComponent>())
	{
		if (const auto rightHand = body->parts.search("Right Hand"))
		{
			auto optionalItem = rightHand->getSlotItem(EquipmentLayer::Held);
			if (optionalItem.has_value())
			{
				body->parts.unequipItem(optionalItem.value());
				auto& throwerPos = actor.get<PositionComponent>();
				auto itemEntity = ItemDatabase::getEntityFromItemID(optionalItem.value());

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
}

drft::math::Range<int> drft::ThrowAbility::getRange(entt::const_handle actor) const
{
	const int maxRange = 12;
	if (auto body = actor.try_get<BodyComponent>())
	{
		if (const auto rightHand = body->parts.search("Right Hand"))
		{
			auto optionalItem = rightHand->getSlotItem(EquipmentLayer::Held);
			if (optionalItem.has_value())
			{
				auto itemEntity = ItemDatabase::getEntityFromItemID(optionalItem.value());
				if (auto material = actor.registry()->try_get<MaterialComponent>(itemEntity))
				{
					int rangeVal = std::max(1, (maxRange - static_cast<int>(material->weight)));
					return { 0, rangeVal };
				}
			}
		}
	}
	return {0, 0};
}

std::vector<sf::Vector2i> drft::ThrowAbility::getTargetingShape(entt::const_handle actor) const
{
	return { {0,0} };
}
