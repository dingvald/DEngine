#include "pch.h"
#include "TilePlacementLayer.h"
#include <ProcGen/Layers/BiomeLayer.h>
#include <Factory/EntityFactory.h>
#include <Components/RenderComponent.h>
#include <Spatial/Conversions.h>
#include <ProcGen/PlaceEntities.h>

using namespace entt::literals;

GenerationState drft::TilePlacementLayerChunk::generate(GenerationLevel /*desiredLevel*/)
{
	// Get biome layer dependency
	auto biomeLayer = generateDependency<BiomeLayer>(_volume);
	if (!biomeLayer.isReady()) return biomeLayer.getState();

	// Place tiles for each position in the chunk
	spatial::forEachPointInRect(_volume.flatten(), [&](sf::Vector2i point) {
		const sf::Vector3i position = { point.x, point.y, _volume.min.z };
		const Biome* biome = biomeLayer.unwrap().getBiomeAt(position);

		sf::Color tileColor = { 10, 10, 10 };
		if (biome)
			tileColor = biome->getBaseTileColor();

		_tiles.emplace_back(position, tileColor);
	});

	return GenerationState::Complete;
}

void drft::TilePlacementLayerChunk::placeTiles(entt::registry& registry) const
{
	auto& factory = _layer.getRegistries().entityFactory;
	for (auto&& [position, color] : _tiles)
	{
		auto tileHandle = gen::placeSingle("tile"_hs, spatial::asTileSpace(position), registry, factory);
		tileHandle.patch<RenderComponent>([&color](RenderComponent& comp) { comp.color = color; });
	}
}

void drft::TilePlacementLayer::placeTiles(spatial::AABB<int> volume, entt::registry& registry)
{
	forEachLoadedChunkInArea(volume.flatten(), volume.min.z,
		[&registry](TilePlacementLayerChunk& chunk) {
			chunk.placeTiles(registry);
		});
	removeChunksInArea(volume.flatten(), volume.min.z);
}

sf::Vector3i drft::TilePlacementLayer::getChunkDimensions() const
{
	return { 8, 8, 8 };
}