#include "pch.h"
#include "CelestialBody.h"

#include <Random/Random.h>
#include "Factory/EntityFactory.h"
#include "ProcGen/PlaceEntities.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"

#include <Cereal/external/rapidjson/document.h>
#include <Components/RenderComponent.h>
#include <EnTT/entt.h>
#include <ProcGen/GenerationState.h>
#include <ProcGen/Layers/BiomeLayer.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <Spatial/AABB.h>
#include <Spatial/ChunkPosition.h>

#include <ProcGen/Layers/VoronoiLayer.h>
#include <ProcGen/Layers/JitteredGridLayer.h>
#include <ProcGen/Layers/RandomLayer.h>
#include <ProcGen/Layers/LayerFactory.h>

#include <ProcGen/GenerationRegistries.h>
#include <ProcGen/EntityPack/EntityPack.h>
#include <ProcGen/LayerPack/LayerPack.h>

#include <Utility/StandardLogger.h>

using namespace drft;

CelestialBody::CelestialBody(const GenerationRegistries& registries)
	: _registries(registries)
{
	// Add generic layers that all generators can use
	_layerManager.add(std::make_unique<RandomLayer>());
	_layerManager.add(std::make_unique<JitteredGridLayer>());
	_layerManager.add(std::make_unique<VoronoiLayer>());
}

GenerationState CelestialBody::generateChunk(drft::ChunkPosition position, entt::registry& registry)
{
	const sf::Vector3i origin = spatial::toTileSpace(position);
	const sf::Vector3i dimensions = spatial::asTileSpace(ChunkDimensions);
	spatial::AABB<int> volume = { origin, dimensions };

	auto layer = _layerManager.generate<BiomeLayer>(volume);
	if (!layer.isReady()) return layer.getState();

	rng::Random random{ std::hash<ChunkPosition>()(position) };
	const auto& factory = registry.ctx().get<const EntityFactory&>();

	// Place tiles
	spatial::forEachPointInRect(volume.flatten(), [&registry, &factory, &layer, z = volume.min.z](sf::Vector2i point)
		{
			const sf::Vector3i point3d = { point.x, point.y, z };
			sf::Color tileColor = { 10,10,10 };
			if (auto biome = layer.unwrap().getBiomeAt(point3d))
			{
				tileColor = biome->getBaseTileColor();
			}
			auto tileHandle = gen::placeSingle("Tile", spatial::asTileSpace(point3d), registry, factory);
			tileHandle.patch<RenderComponent>([&tileColor](RenderComponent& comp) {comp.color = tileColor; });
		});

	// Place Entities from Biomes
	auto bsps = layer.unwrap().getBiomeEntitySlotPointsInArea(volume.flatten(), volume.min);
	for (auto&& [biome, slot, point] : bsps)
	{
		if (auto entity = _entityPacks.selectEntity(slot, random))
		{
			gen::placeSingle(entity.value(), spatial::asTileSpace(sf::Vector3i{point.x, point.y, volume.min.z}), registry, factory);
		}
	}

	return GenerationState::Complete;
}

entt::id_type CelestialBody::getSourceId()
{
	return entt::hashed_string{_name.c_str()};
}

void CelestialBody::tick()
{
}

IChunkGenerator* CelestialBody::tryGetGenerator(entt::id_type sourceId)
{
	for (auto&& child : _celestialBodies)
	{
		if (child.getSourceId() == sourceId) return &child;
		if (auto generator = child.tryGetGenerator(sourceId))
		{
			return generator;
		}
	}
	return nullptr;
}

void CelestialBody::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("name"))
	{
		_name = json["name"].GetString();
	}
	LOG_MSG("Creating celestial body {}", _name);
	if (json.HasMember("layer_packs"))
	{
		for (auto&& val : json["layer_packs"].GetArray())
		{
			entt::id_type packId = entt::hashed_string{ val.GetString() };
			auto& pack = _registries.layerPacks.get(packId);
			for (auto&& [layerId, typeAndparams] : pack.getLayers())
			{
				auto packInstance = _registries.layerFactory.build(typeAndparams.layerTypeId, typeAndparams.json);
				if (!packInstance)
				{
					LOG_WARNING("Could not find layer with id {} in layer factory", typeAndparams.layerTypeId);
					continue;
				}

				_layerManager.add(std::move(packInstance), layerId);
			}
		}
	}
	if (json.HasMember("entity_packs"))
	{
		for (auto&& val : json["entity_packs"].GetArray())
		{
			entt::id_type packId = entt::hashed_string{ val.GetString() };
			auto& pack = _registries.entityPacks.get(packId);
			_entityPacks.add(pack);
		}
	}
	if (json.HasMember("size"))
	{
		if (json["size"].IsArray())
		{
			_sizeRange.setMin(json["size"].GetArray()[0].GetInt());
			_sizeRange.setMax(json["size"].GetArray()[1].GetInt());
		}
		else
		{
			_sizeRange.setMin(json["size"].GetInt());
			_sizeRange.setMax(json["size"].GetInt());
		}
	}
	if (json.HasMember("distance"))
	{
		if (json["distance"].IsArray())
		{
			_distanceRange.setMin(json["distance"].GetArray()[0].GetFloat());
			_distanceRange.setMax(json["distance"].GetArray()[1].GetFloat());
		}
		else
		{
			_distanceRange.setMin(json["distance"].GetFloat());
			_distanceRange.setMax(json["distance"].GetFloat());
		}
	}
	if (json.HasMember("biomes"))
	{
		auto biomeLayer = std::make_unique<BiomeLayer>(_registries.biomes);
		biomeLayer->createFromJson(json);
		_layerManager.add(std::move(biomeLayer));
	}
	if (json.HasMember("bodies"))
	{
		for (auto&& body : json["bodies"].GetArray())
		{
			CelestialBody newBody{_registries};
			newBody.createFromJson(body);
			_celestialBodies.emplace_back(std::move(newBody));
		}
	}
}
