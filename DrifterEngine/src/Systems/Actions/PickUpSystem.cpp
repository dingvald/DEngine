#include "pch.h"
#include "PickUpSystem.h"

#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include <Components/CurrentActorComponent.h>
#include "Components/PositionComponent.h"
#include "Components/ItemComponent.h"
#include "Components/BodyComponent.h"

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include <Systems/Core/ActorSystem.h>
#include <Systems/Helpers/GetDominantSide.h>

#include "Utility/EntityHelpers.h"

void drft::system::PickUpSystem::update()
{
	auto view = _registry.view<component::action::PickUp, PositionComponent, ContainerComponent, CurrentActorComponent>();
	for (auto&& [entity, position, container, currentActor] : view.each())
	{
		if (currentActor.state == CurrentActorState::InProgress) continue;

		if (container.contents.size() >= container.capacity) continue;

		auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
		const auto myTilePosition = position.tile;
		auto checkForItem = [this](entt::entity entity) -> bool
			{
				return _registry.all_of<ItemComponent>(entity);
			};
		const auto items = grid.entitiesAt(myTilePosition, checkForItem);

		if (items.empty()) continue;

		_registry.remove<PositionComponent>(items.front());

		auto& item = _registry.get<ItemComponent>(items.front());

		// Try to put directly into dominant hand
		bool putDirectlyInHand = false;
		if (auto body = _registry.try_get<BodyComponent>(entity))
		{
			if (auto dominantHeld = body->parts.getPartWithSlotType(BodyPart::Slot::Type::Held, util::getDominantSide({ _registry, entity })))
			{
				if (auto slot = dominantHeld->getSlotType(BodyPart::Slot::Type::Held))
				{
					if (slot->item == 0u)
					{
						slot->item = item.id;
						putDirectlyInHand = true;
					}
				}
			}
		}

		// otherwise put into inventory
		if (!putDirectlyInHand)
		{
			_registry.patch<ContainerComponent>(entity,
				[item](ContainerComponent& cont)
				{
					cont.contents.push_back(item.id);
				});
			ActorSystem::setActionComplete({ _registry, entity }, ActionCategory::Act);
		}
	}

	_registry.clear<component::action::PickUp>();
}
