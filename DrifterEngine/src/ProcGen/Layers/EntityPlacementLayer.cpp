#include "pch.h"
#include "EntityPlacementLayer.h"
#include <Random/Random.h>
#include <ProcGen/PlaceEntities.h>
#include <Spatial/Conversions.h>
#include <ProcGen/Layers/FeatureLayer.h>
#include <ProcGen/Layers/EntitySlotLayer.h>
#include <Utility/StandardLogger.h>
#include <Components/StairsComponent.h>

using namespace entt::literals;

namespace Internal
{
	struct StairsPosition
	{
		StairsComponent stairs;
		sf::Vector3i position;
	};
	static bool isChunkAbove(sf::Vector3i myPos, sf::Vector3i otherPos)
	{
		if (myPos.x != otherPos.x || myPos.y != otherPos.y) return false;
		return otherPos.z > myPos.z;
	}
	static bool isChunkBelow(sf::Vector3i myPos, sf::Vector3i otherPos)
	{
		if (myPos.x != otherPos.x || myPos.y != otherPos.y) return false;
		return otherPos.z < myPos.z;
	}
	static std::vector<StairsPosition> tryGetStairs(const EntityPositionMap& entities, const drft::EntityFactory& factory)
	{
		std::vector<StairsPosition> result;
		for (auto&& [pos, entity] : entities)
		{
			auto handle = factory.get(entity);
			if (auto stairs = handle.try_get<StairsComponent>())
			{
				result.emplace_back(*stairs, pos);
			}
		}
		return result;
	}
}

GenerationState drft::EntityPlacementLayerChunk::generate(GenerationLevel desiredLevel)
{
	switch (desiredLevel)
	{
	case drft::GenerationLevel::One:
		return chooseEntitiesForSlots();
	case drft::GenerationLevel::Two:
		return resolveWithNeighborChunks();
	default:
		break;
	}
	return GenerationState::Complete;
}

GenerationState drft::EntityPlacementLayerChunk::chooseEntitiesForSlots()
{
	auto entitySlotLayer = _layer.getLayerManager().generate<EntitySlotLayer>(_volume);
	if (!entitySlotLayer.isReady()) return entitySlotLayer.getState();

	auto featureLayer = _layer.getLayerManager().generate<FeatureLayer>(_volume);
	if (!featureLayer.isReady()) return featureLayer.getState();

	SlotPositionMap slots;
	featureLayer.unwrap().forEachLoadedChunkInVolume(_volume, [&slots](FeatureLayerChunk& chunk) {
			mergeSlotPositionMaps(slots, chunk.slots);
		});
	entitySlotLayer.unwrap().forEachLoadedChunkInVolume(_volume, [&slots](EntitySlotLayerChunk& chunk) {
			mergeSlotPositionMaps(slots, chunk.slots);
		});

	auto* entityPack = _layer.getLayerManager().tryGetEntityPack();
	if (!entityPack)
	{
		LOG_ERROR("Layer manager does not have its EntityPack set. Did you remember to call LayerManager::setEntityPack()?");
		return GenerationState::Failed;
	}

	rng::Random localRandom = { getLocalSeed() };
	
	spatial::forEachPointInRect(_volume.flatten(), [&](sf::Vector2i position) {
		const sf::Vector3i position3d = { position.x, position.y, _volume.min.z };
		if (!slots.contains(position3d)) return true;

		auto slotId = slots.at(position3d).slot;

		auto entityId = entityPack->selectEntity(slotId, localRandom);
		if (!entityId.has_value()) return true;

		chosenEntities.emplace(position3d, entityId.value());
		return true;
	});

	return GenerationState::Complete;
}

GenerationState drft::EntityPlacementLayerChunk::resolveWithNeighborChunks()
{
	GenerationState result = generateNeighborChunks3d(GenerationLevel::One);
	if (result != GenerationState::Complete) return result;

	const EntityPack* entityPack = _layer.getLayerManager().tryGetEntityPack();
	if (!entityPack)
	{
		LOG_ERROR("Layer manager does not have its EntityPack set. Did you remember to call LayerManager::setEntityPack()?");
		return GenerationState::Failed;
	}
	
	// Check for stairs
	forEachLoadedNeighborChunk3d([&](const EntityPlacementLayerChunk& neighbor) {
		if (Internal::isChunkAbove(_index, neighbor._index))
		{
			auto stairs = Internal::tryGetStairs(neighbor.chosenEntities, _layer.getRegistries().entityFactory);
			for (auto&& [stair, position] : stairs)
			{
				if (stair.type != StairsComponent::Type::Down) continue;

				const sf::Vector3i finalPosition{ position.x, position.y, _volume.min.z };
				chosenEntities[finalPosition] = "stairs_up"_hs;
			}
		}
		if (Internal::isChunkBelow(_index, neighbor._index))
		{
			auto stairs = Internal::tryGetStairs(neighbor.chosenEntities, _layer.getRegistries().entityFactory);
			for (auto&& [stair, position] : stairs)
			{
				if (stair.type != StairsComponent::Type::Up) continue;

				const sf::Vector3i finalPosition{ position.x, position.y, _volume.min.z };
				chosenEntities[finalPosition] = "stairs_down"_hs;
			}
		}
	});

	return GenerationState::Complete;
}

void drft::EntityPlacementLayer::placeEntities(spatial::AABB<int> volume, entt::registry& registry)
{
	auto& factory = getRegistries().entityFactory;
	forEachLoadedChunkInArea(volume.flatten(), volume.min.z,
		[&factory, &registry, &volume](EntityPlacementLayerChunk& chunk) {

			for (auto&& [position, entity] : chunk.chosenEntities)
			{
				if (!volume.contains(position)) continue;

				gen::placeSingle(entity, spatial::asTileSpace(position), registry, factory);
			}
		});
}

sf::Vector3i drft::EntityPlacementLayer::getChunkDimensions() const
{
	return { 8, 8, 8 };
}