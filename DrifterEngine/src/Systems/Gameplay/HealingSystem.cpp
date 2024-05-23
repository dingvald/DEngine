#include "pch.h"
#include "HealingSystem.h"
#include "Components/Components.h"
#include "Events/EntityConsumeEvent.h"

void drft::system::HealingSystem::init()
{
	_dispatcher->sink<events::EntityConsumeEvent>().connect<&HealingSystem::onEntityConsumeEvent>(this);
}

void drft::system::HealingSystem::onEntityConsumeEvent(events::EntityConsumeEvent& ev)
{
	if (auto healing = _registry->try_get<component::Healing>(ev.consumed))
	{
		component::action::TakeDamage damage = { .amount = - (healing->amount), .source = ev.consumed };
		_registry->emplace<component::action::TakeDamage>(ev.consumer, std::move(damage));
	}
}
