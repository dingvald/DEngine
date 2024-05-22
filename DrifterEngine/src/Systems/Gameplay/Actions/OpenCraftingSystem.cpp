#include "pch.h"
#include "OpenCraftingSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::OpenCraftingSystem::init()
{
}

void drft::system::OpenCraftingSystem::update(const float dt)
{
	auto view = _registry->view<component::action::OpenCrafting, component::Container>();
	if (view.front() != entt::null)
	{
		_registry->ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::Crafting });
	}
}
