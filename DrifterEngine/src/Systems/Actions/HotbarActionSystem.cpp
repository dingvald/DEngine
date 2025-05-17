#include "pch.h"
#include "HotbarActionSystem.h"

#include <Components/Actions/AbilityAction.h>
#include <Components/CurrentActorComponent.h>
#include "Components/HotbarComponent.h"
#include <Components/Actions/HotbarAction.h>

#include <Events/HUDHotbarPressedEvent.h>

#include <Systems/Helpers/GetPlayerHandle.h>


void drft::system::HotbarActionSystem::init()
{
	_dispatcher.sink<events::HUDHotbarPressedEvent>().connect<&HotbarActionSystem::onHUDHotbarPressedEvent>(this);
}

void drft::system::HotbarActionSystem::update()
{
	auto view = _registry.view<HotbarComponent, HotbarAction, CurrentActorComponent>();
	for (auto&& [entity, hotbar, hotbarAction, currentActor] : view.each())
	{
		if (currentActor.state == CurrentActorState::InProgress) continue;

		entt::handle handle = { _registry, entity };
		entt::id_type abilityId = hotbar.abilities[hotbarAction.index];

		handle.emplace_or_replace<AbilityAction>(abilityId);

		handle.remove<HotbarAction>();
	}
}

void drft::system::HotbarActionSystem::onHUDHotbarPressedEvent(const events::HUDHotbarPressedEvent& ev)
{
	if (auto player = getPlayerHandle(_registry))
	{
		player.emplace_or_replace<HotbarAction>(ev.index);
	}
}
