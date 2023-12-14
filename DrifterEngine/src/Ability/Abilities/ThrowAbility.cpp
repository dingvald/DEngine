#include "pch.h"
#include "ThrowAbility.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Spatial/Helpers.h"

sf::Color drft::ThrowAbility::getIconColor() const
{
	return sf::Color::Magenta;
}

int drft::ThrowAbility::getSpriteIndex() const
{
	return 2;
}

drft::AbilityTargetingType drft::ThrowAbility::getTargetingType() const
{
	return AbilityTargetingType::SelectSquare;
}

bool drft::ThrowAbility::isValid(entt::const_handle actor) const
{
	if (auto body = actor.try_get<component::Body>())
	{
		if (const auto rightHand = body->parts.search("Right Hand"))
		{
			auto optionalItem = rightHand->getSlotItem(EquipmentLayer::Held);
			if (optionalItem.has_value()) return true;
		}
	}
	return false;
}

void drft::ThrowAbility::perform(entt::handle actor, std::optional<sf::Vector2i> targetPosition) const
{
	const float throwSpeed = 6.0f;
	if (!targetPosition.has_value()) throw std::exception("You need a target to throw at.");
	if (auto body = actor.try_get<component::Body>())
	{
		if (const auto rightHand = body->parts.search("Right Hand"))
		{
			auto optionalItem = rightHand->getSlotItem(EquipmentLayer::Held);
			if (optionalItem.has_value())
			{
				body->parts.unequipItem(optionalItem.value());
				auto& throwerPos = actor.get<component::Position>();
				auto line = spatial::getIntPointsAlongLine(throwerPos.position, targetPosition.value());
				
				auto itemEntity = ItemDatabase::getEntityFromItemID(optionalItem.value());
				actor.registry()->emplace<component::Position>(itemEntity, line.front());
				actor.registry()->emplace<component::Projectile>(itemEntity, std::move(line), 1, throwSpeed);
			}
		}
	}
}

drft::math::Range<int> drft::ThrowAbility::getRange(entt::const_handle actor) const
{
	return {0, 9};
}

std::vector<sf::Vector2i> drft::ThrowAbility::getTargetingShape(entt::const_handle actor) const
{
	return { {0,0} };
}
