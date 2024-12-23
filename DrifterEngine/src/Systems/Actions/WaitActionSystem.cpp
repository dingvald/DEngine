#include "pch.h"
#include "WaitActionSystem.h"
#include "Components/Components.h"
#include "Components/CurrentActorComponent.h"
#include "Components/PositionComponent.h"
#include "Components/Actions/WaitAction.h"
#include "Components/Tags.h"

#include "Systems/Core/ActorSystem.h"

void drft::system::WaitActionSystem::update()
{
	auto view = _registry.view<WaitAction, CurrentActorComponent, component::tag::Active>();
	for (auto&& [entity, currentActor] : view.each())
	{
		entt::handle handle = { _registry, entity };

		handle.emplace_or_replace<component::action::ConsumeStamina>(-0.5f);
		ActorSystem::completeAction(handle, ActionCategory::None);
		handle.remove<WaitAction>();
	}
}
