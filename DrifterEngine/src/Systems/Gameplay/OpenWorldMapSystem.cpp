#include "pch.h"
#include "OpenWorldMapSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::OpenWorldMapSystem::init()
{
}

void drft::system::OpenWorldMapSystem::update(const float dt)
{
	auto view = registry->view<component::action::OpenWorldMap>();
	if (view.front() != entt::null)
	{
		registry->ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::Map });
	}
}
