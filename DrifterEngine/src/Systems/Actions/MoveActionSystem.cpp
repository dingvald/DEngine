#include <pch.h>
#include "MoveActionSystem.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include <Spatial/Helpers.h>

#include "Components/Components.h"
#include "Components/CollisionComponent.h"
#include "Components/CurrentActorComponent.h"
#include "Components/Actions/MoveAction.h"
#include "Components/PhysicalBlockingComponent.h"
#include "Components/PositionComponent.h"
#include "Components/StaminaComponent.h"
#include <Components/TweeningComponent.h>

#include <Systems/Core/TweeningSystem.h>
#include <Systems/Core/ActorSystem.h>
#include "Utility/EntityHelpers.h"

void drft::system::MoveActionSystem::init()
{
	_registry.on_construct<MoveAction>().connect<&MoveActionSystem::onMoveActionAdded>(this);
}

void drft::system::MoveActionSystem::update()
{
	auto view = _registry.view<PositionComponent, MoveAction>();
	for (auto&& [entity, position, move] : view.each())
	{
		Tween moveToTween = {
			.targetOffset = spatial::toFloatSpace(spatial::asTileSpace(move.direction)) * 1.0f,
			.time = 0.05f,
			.easing = Easing::linear,
			.onFinish = [this, action = move](entt::handle entity) {
				processMoveAction(entity, std::move(action));
			}
		};

		entt::handle handle = { _registry, entity };
		TweeningSystem::tween(handle, moveToTween);

		ActorSystem::setActionInProgress(handle);
		_registry.remove<MoveAction>(entity);
	}
}

void drft::system::MoveActionSystem::onMoveActionAdded(entt::registry& registry, entt::entity entity) const
{
	const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
	const auto& positionComponent = registry.get<PositionComponent>(entity);
	auto& moveAction = registry.get<MoveAction>(entity);

	sf::Vector3i targetPosition = positionComponent.tile + spatial::vec3FromPlanar(moveAction.direction);
	auto checkForBlockers = [this](entt::entity entity) -> bool
		{
			return _registry.all_of<PhysicalBlockingComponent>(entity);
		};
	auto blockers = grid.entitiesAt(spatial::asTileSpace(targetPosition), checkForBlockers);

	if (!blockers.empty())
	{
		_registry.emplace_or_replace<CollisionComponent>(entity, moveAction.direction, std::move(blockers));
		_registry.remove<MoveAction>(entity);
	}
}

void drft::system::MoveActionSystem::processMoveAction(entt::entity entity, MoveAction action) const
{
	entt::handle handle = { _registry, entity };
	if (action.direction == sf::Vector2i{ 0,0 })
	{
		ActorSystem::setActionComplete(handle, ActionCategory::None, 0);
	}

	move(handle, action.direction);
	ActorSystem::setActionComplete(handle, ActionCategory::Move);
}

void drft::system::MoveActionSystem::move(entt::handle entity, sf::Vector2i direction) const
{
	TilePosition tileDirection = spatial::asTileSpace(direction);
	entity.patch<PositionComponent>([tileDirection](PositionComponent& position) {
			position.tile += tileDirection;
		}
	);
}
