#include "pch.h"
#include "TargetSelectSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::TargetSelectSystem::init()
{
	_registry->on_construct<component::action::SelectTarget>().connect<&TargetSelectSystem::onTargetSelect>(this);
}

void drft::system::TargetSelectSystem::onTargetSelect(entt::registry& registry, entt::entity entity)
{
	registry.ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::SelectTarget });
}
