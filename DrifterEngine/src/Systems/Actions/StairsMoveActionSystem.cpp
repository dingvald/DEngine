#include "pch.h"
#include "StairsMoveActionSystem.h"
#include <Spatial/WorldGrid.h>
#include <Components/Actions/MoveDownStairsAction.h>
#include <Components/Actions/MoveUpStairsAction.h>
#include <Components/PositionComponent.h>
#include <Components/StairsComponent.h>
#include <Events/PlayerTransferRequestEvent.h>
#include <Spatial/Conversions.h>
#include <spatial/ChunkPosition.h>

#include <Utility/StandardLogger.h>

#pragma optimize("", off)

static const drft::TilePosition OneChunkDown = drft::spatial::toTileSpace(drft::ChunkPosition{ 0, 0, -1 });
static const drft::TilePosition OneChunkUp = drft::spatial::toTileSpace(drft::ChunkPosition{ 0, 0, 1 });

using namespace entt::literals;

void drft::system::StairsMoveActionSystem::init()
{
	_registry.on_construct<MoveDownStairsAction>().connect<&StairsMoveActionSystem::onMoveDownStairsActionAdded>(this);
	_registry.on_construct<MoveUpStairsAction>().connect<&StairsMoveActionSystem::onMoveUpStairsActionAdded>(this);
}

void drft::system::StairsMoveActionSystem::update()
{
	_registry.clear<MoveDownStairsAction>();
	_registry.clear<MoveUpStairsAction>();
}

void drft::system::StairsMoveActionSystem::onMoveUpStairsActionAdded(entt::registry& registry, entt::entity entity) const
{
	if (auto position = registry.try_get<PositionComponent>(entity))
	{
		auto& worldGrid = _registry.ctx().get<spatial::WorldGrid>();
		auto stairsEntities = worldGrid.entitiesAt(position->tile, [&registry](entt::entity entity) { return registry.all_of<StairsComponent>(entity); });
		if (stairsEntities.empty()) return;

		auto& stairsComponent = registry.get<StairsComponent>(stairsEntities.front());
		if (stairsComponent.type != StairsComponent::Type::Up) return;

		LOG_MSG("Stairs up here!");
	}
}

void drft::system::StairsMoveActionSystem::onMoveDownStairsActionAdded(entt::registry& registry, entt::entity entity) const
{
	if (auto position = registry.try_get<PositionComponent>(entity))
	{
		auto& worldGrid = _registry.ctx().get<spatial::WorldGrid>();
		auto stairsEntities = worldGrid.entitiesAt(position->tile, [&registry](entt::entity entity) { return registry.all_of<StairsComponent>(entity); });
		if (stairsEntities.empty()) return;

		auto& stairsComponent = registry.get<StairsComponent>(stairsEntities.front());
		if (stairsComponent.type != StairsComponent::Type::Down) return;

		_dispatcher.trigger(events::PlayerTransferRequestEvent{ .position = position->tile + OneChunkDown });

		///_registry.patch<PositionComponent>(entity, [](PositionComponent& position) {
		///	position.tile += OneChunkDown;
		///});
	}
}
