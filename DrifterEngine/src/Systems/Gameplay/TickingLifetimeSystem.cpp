#include "pch.h"
#include "TickingLifetimeSystem.h"
#include "Components/Components.h"

void drft::system::TickingLifetimeSystem::init()
{
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::GameTickEvent>().connect<&TickingLifetimeSystem::onGameTickEvent>(this);
}

void drft::system::TickingLifetimeSystem::onGameTickEvent(events::GameTickEvent& ev)
{
	auto view = registry->view<component::TickingLifetime>();
	for (auto [entity, lifetime] : view.each())
	{
		--lifetime.ticksRemaining;
		if (lifetime.ticksRemaining <= 0)
		{
			registry->destroy(entity);
		}
	}
}
