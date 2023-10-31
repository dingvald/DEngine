#include "pch.h"
#include "PickUpSystem.h"
#include "Components/Components.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Utility/EntityHelpers.h"

void drft::system::PickUpSystem::init()
{
}

void drft::system::PickUpSystem::update(const float dt)
{
	auto view = registry->view<component::action::PickUp, component::Position, component::Container>();
	auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	for (auto [entity, pos, container] : view.each())
	{
		if (container.contents.size() >= container.capacity) continue;

		const auto myTilePosition = pos.position;
		const auto items = grid.entitiesAt(myTilePosition, [this](entt::entity entity) -> bool
			{
				return registry->all_of<component::Item>(entity);
			});

		if (!items.empty())
		{
			registry->remove<component::Position>(items.front());

			auto& item = registry->get<component::Item>(items.front());

			bool putDirectlyInHand = false;
			if (auto body = registry->try_get<component::Body>(entity))
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
				registry->patch<component::Container>(entity,
					[item](component::Container& cont)
					{
						cont.contents.push_back(item.id);
					});
			}
			spendActionPoints(*registry, entity, ActionType::Act);
		}
	}
}

void drft::system::PickUpSystem::onUpdateEnd()
{
	registry->clear<component::action::PickUp>();
}
