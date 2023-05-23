#include "pch.h"
#include "PickUpSystem.h"
#include "Components/Components.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
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
		const auto myTilePosition = spatial::toTileSpace(pos.position);
		const auto items = grid.entitiesAt(myTilePosition);

		component::Item* itemComp = nullptr;
		entt::entity pickupItem = entt::null;

		for (auto item : items)
		{
			if (itemComp = registry->try_get<component::Item>(item))
			{
				pickupItem = item;
				break;
			}
		}
		if (!itemComp)
		{
			std::cout << "Nothing to pick up." << std::endl;
		}
		else
		{
			registry->remove<component::Position>(pickupItem);
			std::cout << util::getEntityName({ *registry, entity }) 
				<< " picked up a " << util::getEntityName({ *registry, pickupItem }) << std::endl;

			bool putDirectlyInHand = false;
			// try to put in right hand first (NOT LEFT HAND!)
			if (auto body = registry->try_get<component::Body>(entity))
			{
				if (body->parts.contains("HeldR") && body->parts.at("HeldR") == component::Item::NONE)
				{
					body->parts.at("HeldR") = itemComp->id;
					putDirectlyInHand = true;
				}
			}
			// otherwise put into inventory
			if (!putDirectlyInHand)
			{
				registry->patch<component::Container>(entity,
					[itemComp](component::Container& cont)
					{
						cont.contents.push_back(itemComp->id);
					});
			}

			const int actionCost = util::getActionCost({ *registry, entity }, 100, util::ActionType::Act);
			registry->emplace_or_replace<component::action::SpendPoints>(entity, actionCost);
		}

		registry->remove<component::action::PickUp>(entity);
	}
}
