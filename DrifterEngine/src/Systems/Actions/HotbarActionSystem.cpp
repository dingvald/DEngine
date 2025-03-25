#include "pch.h"
#include "HotbarActionSystem.h"

#include <Components/Actions/AbilityAction.h>
#include <Components/CurrentActorComponent.h>
#include "Components/PlayerInputComponent.h"
#include "Components/HotbarComponent.h"
#include <Components/Actions/HotbarAction.h>
#include "Components/PositionComponent.h"

#include "Components/Tags.h"

#include <Events/HUDHotbarPressedEvent.h>

#include "Systems/Helpers/ToHotbarIndex.h"
#include <Systems/Helpers/GetPlayerHandle.h>


void drft::system::HotbarActionSystem::init()
{
	_dispatcher.sink<events::HUDHotbarPressedEvent>().connect<&HotbarActionSystem::onHUDHotbarPressedEvent>(this);
}

void drft::system::HotbarActionSystem::start()
{
	auto playerView = _registry.view<PlayerInputComponent>();
	for (auto entity : playerView)
	{
		if (_registry.all_of<HotbarComponent>(entity)) continue;

		auto& hotbar = _registry.emplace<HotbarComponent>(entity);
		// TODO: For testing purposes only
		hotbar.abilities[fromHotbarIndex(1)] = AbilityType::Sprint;
		hotbar.abilities[fromHotbarIndex(2)] = AbilityType::Throw;
	}
}

void drft::system::HotbarActionSystem::update()
{
	auto view = _registry.view<HotbarComponent, HotbarAction, CurrentActorComponent>();
	for (auto&& [entity, hotbar, hotbarSlot, currentActor] : view.each())
	{
		if (currentActor.state == CurrentActorState::InProgress) continue;

		entt::handle handle = { _registry, entity };
		AbilityType abilityType = hotbar.abilities[hotbarSlot.index];

		//TODO: add ability targeting here - this is really only used by player anyways

		handle.emplace_or_replace<AbilityAction>(abilityType);

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
