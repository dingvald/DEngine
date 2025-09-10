#include "pch.h"
#include "MouseActionSystem.h"

#include <Components/Actions/InspectAction.h>
#include <Components/Actions/MouseContextAction.h>
#include <Components/Actions/MouseInspectAction.h>
#include <Components/MouseCursorComponent.h>
#include <Components/PositionComponent.h>
#include <Components/PathNavComponent.h>

#include <Systems/Helpers/EmplacePathNavToPosition.h>
#include <Systems/Helpers/GetMouseHandle.h>
#include <Systems/Core/MouseVisualizationSystem.h>

void drft::system::MouseActionSystem::init()
{
	_registry.on_construct<MouseContextAction>().connect<&MouseActionSystem::onMouseContextActionAdded>(this);
	_registry.on_construct<MouseInspectAction>().connect<&MouseActionSystem::onMouseInspectActionAdded>(this);
}

void drft::system::MouseActionSystem::update(const float dt)
{
	_registry.clear<MouseContextAction>();
	_registry.clear<MouseInspectAction>();
}

void drft::system::MouseActionSystem::onMouseContextActionAdded(entt::registry& registry, entt::entity entity) const
{
	auto mouse = getMouseConstHandle(registry);

	auto mouseCursorComponent = mouse.try_get<MouseCursorComponent>();
	auto mousePositionComponent = mouse.try_get<PositionComponent>();

	if (!mouseCursorComponent || !mousePositionComponent) return;

	switch (mouseCursorComponent->actionState)
	{
	case MouseContextualActionState::Move:
	{
		auto path = MouseVisualizationSystem::getVisualizedPath(_registry);
		registry.emplace_or_replace<PathNavComponent>(entity, std::move(path));
		break;
	}	
	case MouseContextualActionState::Attack:
	{
		auto path = MouseVisualizationSystem::getVisualizedPath(_registry);
		registry.emplace_or_replace<PathNavComponent>(entity, std::move(path));
		break;
	}
	case MouseContextualActionState::Interact:
		std::cout << "Interact action" << std::endl;
		break;
	default:
		throw std::exception("Unhandled enum case in MouseActionSystem");
		break;
	}
}

void drft::system::MouseActionSystem::onMouseInspectActionAdded(entt::registry& registry, entt::entity entity) const
{
	auto mouse = getMouseConstHandle(registry);
	if (auto positionComponent = mouse.try_get<PositionComponent>())
	{
		registry.emplace_or_replace<InspectAction>(entity, positionComponent->tile);
	}
}

