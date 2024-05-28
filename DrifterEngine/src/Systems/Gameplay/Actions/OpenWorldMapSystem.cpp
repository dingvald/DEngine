#include "pch.h"
#include "OpenWorldMapSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::OpenWorldMapSystem::init()
{
	_registry->on_construct<component::action::OpenWorldMap>().connect<&OpenWorldMapSystem::onOpenWorldMapAction>(this);
}

void drft::system::OpenWorldMapSystem::onOpenWorldMapAction(entt::registry& registry, entt::entity entity) const
{
	_dispatcher->trigger(events::RequestStateStackPush{ States::Map });
}
