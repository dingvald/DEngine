#include "pch.h"
#include "HotbarSystem.h"

#include <Components/Actions/AbilityAction.h>
#include "Components/Components.h"
#include <Components/CurrentActorComponent.h>
#include "Components/PlayerComponent.h"
#include "Components/HotbarComponent.h"
#include "Components/PositionComponent.h"

#include "Components/Tags.h"

#include "Systems/Helpers/ToHotbarIndex.h"


void drft::system::HotbarSystem::onStart()
{
	auto playerView = _registry.view<PlayerComponent>();
	for (auto entity : playerView)
	{
		if (_registry.all_of<HotbarComponent>(entity)) continue;

		auto& hotbar = _registry.emplace<HotbarComponent>(entity);
		// TODO: For testing purposes only
		hotbar.abilities[toHotbarIndex(1)] = AbilityType::Sprint;
		hotbar.abilities[toHotbarIndex(2)] = AbilityType::Throw;
	}
}

void drft::system::HotbarSystem::onUpdate(float dt)
{
	auto view = _registry.view<HotbarComponent, component::action::HotbarPressed, CurrentActorComponent>();
	for (auto&& [entity, hotbar, hotbarSlot, currentActor] : view.each())
	{
		entt::handle handle = { _registry, entity };
		AbilityType abilityType = hotbar.abilities[hotbarSlot.slot];

		handle.emplace_or_replace<AbilityAction>(abilityType);

		handle.remove<component::action::HotbarPressed>();
	}
}
