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
	static std::vector<StairsPosition> tryGetStairs(const EntityPositionList& entities, const drft::EntityFactory& factory)
	{
		std::vector<StairsPosition> result;
		for (auto&& [entity, position] : entities)
		{
			auto handle = factory.get(entity);
			if (auto stairs = handle.try_get<StairsComponent>())
			{
				result.emplace_back(*stairs, position);
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
		break;
	case drft::GenerationLevel::Two:
		return resolveWithNeighborChunks();
		break;
	default:
		break;
	}
	return GenerationState::Complete;
}

GenerationState drft::EntityPlacementLayerChunk::chooseEntitiesForSlots()
{
	// Both of these layers add to the slot_canvas, so they are dependencies
	auto entitySlotLayer = _layer.getLayerManager().generate<EntitySlotLayer>(_volume);
	if (!entitySlotLayer.isReady()) return entitySlotLayer.getState();

	auto featureLayer = _layer.getLayerManager().generate<FeatureLayer>(_volume.expand({ 50.f, 50.f, 1.f }));
	if (!featureLayer.isReady()) return featureLayer.getState();

	auto& slotCanvas = _layer.getLayerManager().getCanvas("slot_canvas"_hs);
	auto* entityPack = _layer.getLayerManager().tryGetEntityPack();
	if (!entityPack)
	{
		LOG_ERROR("Layer manager does not have its EntityPack set. Did you remember to call LayerManager::setEntityPack()?");
		return GenerationState::Failed;
	}

	rng::Random localRandom = { getLocalSeed() };

	spatial::forEachPointInRect(_volume.flatten(), [&](sf::Vector2i position) {
		const sf::Vector3i position3d = { position.x, position.y, _volume.min.z };
		auto slotId = slotCanvas.get(position3d);
		if (!slotId.has_value()) return;

		auto entityId = entityPack->selectEntity(std::any_cast<entt::id_type>(slotId), localRandom);
		if (!entityId.has_value()) return;

		chosenEntities.emplace_back(entityId.value(), position3d);
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
				if (stair.type == StairsComponent::Type::Down)
				{
					chosenEntities.emplace_back("stairs_up"_hs, sf::Vector3i{ position.x, position.y, _volume.min.z });
				}
			}
		}
		if (Internal::isChunkBelow(_index, neighbor._index))
		{
			auto stairs = Internal::tryGetStairs(neighbor.chosenEntities, _layer.getRegistries().entityFactory);
			for (auto&& [stair, position] : stairs)
			{
				if (stair.type == StairsComponent::Type::Up)
				{
					chosenEntities.emplace_back("stairs_down"_hs, sf::Vector3i{ position.x, position.y, _volume.min.z });
				}
			}
		}
	});

	return GenerationState::Complete;
}

void drft::EntityPlacementLayer::placeEntities(spatial::AABB<int> volume, entt::registry& registry)
{
	auto& factory = getRegistries().entityFactory;
	forEachLoadedChunkInArea(volume.flatten(), volume.min.z,
		[&factory, &registry](EntityPlacementLayerChunk& chunk) {
			for (auto&& [entity, position] : chunk.chosenEntities)
			{
				gen::placeSingle(entity, spatial::asTileSpace(position), registry, factory);
			}
		});
	removeChunksInArea(volume.flatten(), volume.min.z);
}

sf::Vector3i drft::EntityPlacementLayer::getChunkDimensions() const
{
	return { 8, 8, 8 };
}