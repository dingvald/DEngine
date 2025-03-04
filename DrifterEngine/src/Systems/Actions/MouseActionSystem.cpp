#include "pch.h"
#include "MouseActionSystem.h"

#include <Components/Actions/MouseAction.h>

void drft::system::MouseActionSystem::init()
{
	_registry.on_construct<MouseAction>().connect<&MouseActionSystem::onMouseActionAdded>(this);
}

void drft::system::MouseActionSystem::update()
{
	_registry.clear<MouseAction>();
}

void drft::system::MouseActionSystem::onMouseActionAdded(entt::registry& registry, entt::entity entity) const
{
	auto& mouseAction = registry.get<MouseAction>(entity);

}

