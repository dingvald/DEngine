#include "pch.h"
#include "PickUpSystem.h"

#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ItemComponent.h"
#include "Components/BodyComponent.h"

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Utility/EntityHelpers.h"

void drft::system::PickUpSystem::init()
{
	_registry->on_construct<component::action::PickUp>().connect<&PickUpSystem::onPickupAction>(this);
}

void drft::system::PickUpSystem::onUpdateEnd()
{
	_registry->clear<component::action::PickUp>();
}

void drft::system::PickUpSystem::onPickupAction(entt::registry& registry, entt::entity entity) const
{
	auto container = registry.try_get<ContainerComponent>(entity);
	auto position = registry.try_get<PositionComponent>(entity);

	if (!container || !position) return;
	if (container->contents.size() >= container->capacity) return;

	auto& grid = registry.ctx().get<spatial::WorldGrid&>();
	const auto myTilePosition = position->position;
	auto checkForItem = [this](entt::entity entity) -> bool
	{
		return _registry->all_of<ItemComponent>(entity);
	};
	const auto items = grid.entitiesAt(myTilePosition, checkForItem);

	if (items.empty()) return;

	_registry->remove<PositionComponent>(items.front());

	auto& item = _registry->get<ItemComponent>(items.front());

	bool putDirectlyInHand = false;
	if (auto body = _registry->try_get<BodyComponent>(entity))
	{
		const auto handParts = body->parts.search(PartType::Hand);
		for (auto hand : handParts)
		{
			if (!hand->getSlotItem(EquipmentLayer::Held).has_value())
			{
				hand->addSlotItem(item.id, EquipmentLayer::Held);
				putDirectlyInHand = true;
				break;
			}
		}
	}
	// otherwise put into inventory
	if (!putDirectlyInHand)
	{
		_registry->patch<ContainerComponent>(entity,
			[item](ContainerComponent& cont)
			{
				cont.contents.push_back(item.id);
			});
	}

	spendActionPoints(BASE_ACTION_COST, ActionType::Act, { *_registry, entity });
}
