#include "pch.h"
#include "OpenCraftingSystem.h"
#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include "Events/RequestStateChange.h"

void drft::system::OpenCraftingSystem::init()
{
	_registry->on_construct<component::action::OpenCrafting>().connect<&OpenCraftingSystem::onOpenCraftingAction>(this);
}

void drft::system::OpenCraftingSystem::onOpenCraftingAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<ContainerComponent>(entity))
	{
		registry.remove<component::action::OpenCrafting>(entity);
		return;
	}

	_dispatcher->trigger(events::RequestStateStackPush{ States::Crafting });
}
