#include "pch.h"
#include "WaitingSystem.h"
#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/Tags.h"

void drft::system::WaitingSystem::init()
{
	_registry->on_construct<component::action::Wait>().connect<&WaitingSystem::onWaitAction>(this);
}

void drft::system::WaitingSystem::onUpdateEnd()
{
	_registry->clear<component::action::Wait>();
}

void drft::system::WaitingSystem::onWaitAction(entt::registry& registry, entt::entity entity) const
{
	if (!registry.all_of<PositionComponent, component::tag::Active>(entity))
	{
		registry.remove<component::action::Wait>(entity);
		return;
	}

	registry.emplace_or_replace<component::action::ConsumeStamina>(entity, -0.5f);
	registry.emplace_or_replace<component::action::SpendPoints>(entity, 100);
}
