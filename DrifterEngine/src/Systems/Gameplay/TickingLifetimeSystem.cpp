#include "pch.h"
#include "TickingLifetimeSystem.h"
#include "Components/Components.h"

void drft::system::TickingLifetimeSystem::init()
{
	auto& dispatcher = _registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::GameTickEvent>().connect<&TickingLifetimeSystem::onGameTickEvent>(this);
}

void drft::system::TickingLifetimeSystem::onGameTickEvent(events::GameTickEvent& ev)
{
	// Has position so that items in inventory don't tick their lifetimes
	auto view = _registry->view<component::TickingLifetime, component::Position>();
	for (auto [entity, lifetime, pos] : view.each())
	{
		--lifetime.ticksRemaining;
		if (lifetime.ticksRemaining <= 0)
		{
			_registry->destroy(entity);
		}
	}
}
