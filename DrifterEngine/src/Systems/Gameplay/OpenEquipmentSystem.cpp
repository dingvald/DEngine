#include "pch.h"
#include "OpenEquipmentSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::OpenEquipmentSystem::init()
{
}

void drft::system::OpenEquipmentSystem::update(const float dt)
{
	auto view = registry->view<component::action::OpenEquipment, component::Container>();
	if (view.front() != entt::null)
	{
		registry->ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::Inventory });
	}
}

