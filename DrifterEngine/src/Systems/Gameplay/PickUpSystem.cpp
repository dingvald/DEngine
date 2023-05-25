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
		const auto myTilePosition = spatial::toTileSpace(pos.position);
		const auto items = grid.entitiesAt(myTilePosition,
			[this](entt::entity entity) -> bool
			{
				return registry->all_of<component::Item>(entity);
			});

		if (items.empty())
		{
			std::cout << "Nothing to pick up." << std::endl;
		}
		else
		{
			registry->remove<component::Position>(items.front());
			std::cout << util::getEntityName({ *registry, entity }) 
				<< " picked up a " << util::getEntityName({ *registry, items.front()}) << std::endl;

			auto item = registry->get<component::Item>(items.front());

			bool putDirectlyInHand = false;
			// try to put in right hand first (NOT LEFT HAND!)
			if (auto body = registry->try_get<component::Body>(entity))
			{
				if (body->parts.contains("HeldR") && body->parts.at("HeldR") == component::Item::NONE)
				{
					body->parts.at("HeldR") = item.id;
					putDirectlyInHand = true;
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
