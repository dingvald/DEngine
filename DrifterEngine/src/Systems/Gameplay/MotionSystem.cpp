#include "pch.h"
#include "MotionSystem.h"

#include "Components/Components.h"
#include "Components/MotionComponent.h"
#include "Components/PositionComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/CollisionComponent.h"

#include "Spatial/WorldGrid.h"

void drft::system::MotionSystem::init()
{
	_registry->on_construct<MotionComponent>().connect<&MotionSystem::onMotionAdded>(this);
}

void drft::system::MotionSystem::onUpdateEnd()
{
	_registry->clear<MotionComponent>();
}

void drft::system::MotionSystem::onMotionAdded(entt::registry& registry, entt::entity entity) const
{
	const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	const auto& motionComponent = registry.get<MotionComponent>(entity);
	const auto& positionComponent = registry.get<PositionComponent>(entity);
	
	sf::Vector2i targetPosition = positionComponent.position + motionComponent.direction;
	auto checkForBlockers = [this](entt::entity entity) -> bool
	{
		if (auto material = _registry->try_get<MaterialComponent>(entity))
		{
			return material->blocks;
		}
		return false;
	};
	auto blockers = grid.entitiesAt(targetPosition, checkForBlockers);

	if (blockers.empty())
	{
		_registry->emplace_or_replace<component::action::DoMove>(entity);
	}
	else
	{
		_registry->emplace_or_replace<CollisionComponent>(entity, motionComponent.direction, std::move(blockers));
	}
}
