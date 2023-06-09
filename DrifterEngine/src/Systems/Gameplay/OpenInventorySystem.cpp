#include "pch.h"
#include "OpenInventorySystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"
#include "Utility/EntityHelpers.h"

void drft::system::OpenInventorySystem::init()
{}

void drft::system::OpenInventorySystem::update(const float dt)
{
	auto view = registry->view<component::action::OpenInventory, component::Container>();
	if (view.front() != entt::null)
	{
		registry->ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::Inventory });
	}
}
