#include "pch.h"
#include "TickingLifetimeSystem.h"
#include "Components/PositionComponent.h"
#include "Components/TickingLifetimeComponent.h"

void drft::system::TickingLifetimeSystem::init()
{
	_dispatcher->sink<events::GameTickEvent>().connect<&TickingLifetimeSystem::onGameTickEvent>(this);
}

void drft::system::TickingLifetimeSystem::onGameTickEvent(events::GameTickEvent& ev)
{
	// Has position so that items in inventory don't tick their lifetimes
	auto view = _registry->view<TickingLifetimeComponent, PositionComponent>();
	for (auto [entity, lifetime, pos] : view.each())
	{
		--lifetime.ticksRemaining;
		if (lifetime.ticksRemaining <= 0)
		{
			_registry->destroy(entity);
		}
	}
}
