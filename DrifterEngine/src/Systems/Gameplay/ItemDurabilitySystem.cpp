#include "pch.h"
#include "ItemDurabilitySystem.h"
#include "Components/Components.h"

void drft::system::ItemDurabilitySystem::init()
{
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::ItemBreakEvent>().connect<&ItemDurabilitySystem::onItemBreakEvent>(this);
}

void drft::system::ItemDurabilitySystem::onItemBreakEvent(events::ItemBreakEvent& ev)
{
	// find owner
	auto bodyView = registry->view<component::Body>();
	for (auto [entity, body] : bodyView.each())
	{
		for (auto& [slotname, itemID] : body.parts)
		{
			if (itemID == ev.itemID)
			{
				itemID = component::Item::NONE;
				return;
			}
		}
	}
}
