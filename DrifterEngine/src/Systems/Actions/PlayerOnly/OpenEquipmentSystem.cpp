#include "pch.h"
#include "OpenEquipmentSystem.h"
#include "Components/Components.h"
#include "Components/ContainerComponent.h"
#include "Events/RequestStateChange.h"

void drft::system::OpenEquipmentSystem::init()
{
	_registry.on_construct<component::action::OpenEquipment>().connect<&OpenEquipmentSystem::onOpenEquipmentAction>(this);
}

void drft::system::OpenEquipmentSystem::onOpenEquipmentAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<ContainerComponent>(entity))
	{
		registry.remove<component::action::OpenEquipment>(entity);
		return;
	}

	_dispatcher.trigger(events::RequestStateStackPush{ States::Inventory });
}

