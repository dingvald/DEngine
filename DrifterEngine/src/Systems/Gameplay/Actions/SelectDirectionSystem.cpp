#include "pch.h"
#include "SelectDirectionSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::SelectDirectionSystem::init()
{
	_registry.on_construct<component::action::SelectDirection>().connect<&SelectDirectionSystem::onSelectDirection>(this);
}

void drft::system::SelectDirectionSystem::onSelectDirection(entt::registry& registry, entt::entity entity)
{
	_dispatcher.trigger(events::RequestStateStackPush{ States::SelectDirection });
}
