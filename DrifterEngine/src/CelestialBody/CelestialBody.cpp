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
#include <ProcGen/Layers/PerlinNoiseLayer.h>
#include <ProcGen/Layers/LloydRelaxedLayer.h>
#include <ProcGen/Layers/FillLayer.h>
#include <ProcGen/Layers/DepthLayer.h>
#include <ProcGen/Layers/FeatureLayer.h>
#include <ProcGen/Layers/EntitySlotLayer.h>
#include <ProcGen/Layers/EntityPlacementLayer.h>
#include <ProcGen/Layers/TilePlacementLayer.h>

#include <ProcGen/GenerationRegistries.h>
#include <ProcGen/EntityPack/EntityPack.h>
#include <ProcGen/LayerPack/LayerPack.h>

#include <Utility/StandardLogger.h>

using namespace drft;
using namespace entt::literals;

namespace Internal
{
	static void placeTile(sf::Vector3i position, const Biome* biome, entt::registry& registry, const drft::EntityFactory& factory)
	{
		sf::Color tileColor = { 10,10,10 };
		if (biome)
		{
			tileColor = biome->getBaseTileColor();
		}
		auto tileHandle = gen::placeSingle("tile"_hs, spatial::asTileSpace(position), registry, factory);
		tileHandle.patch<RenderComponent>([&tileColor](RenderComponent& comp) {comp.color = tileColor; });
	}
}

CelestialBody::CelestialBody(const GenerationRegistries& registries)
	: _registries(registries)
	, _layerManager(registries)
{
	using namespace entt::literals;
	// register layer types that can have multiple instances created using add
	_layerManager.registerType<drft::RandomLayer>("random"_hs);
	_layerManager.registerType<drft::PerlinNoiseLayer>("perlin"_hs);
	_layerManager.registerType<drft::LloydRelaxedLayer>("relaxed_points"_hs);
	_layerManager.registerType<drft::FillLayer>("fill"_hs);
	_layerManager.registerType<drft::DepthLayer>("depth"_hs);

	// Add generic layers that all generators can use
	_layerManager.add<RandomLayer>();
	_layerManager.add<JitteredGridLayer>();
	_layerManager.add<VoronoiLayer>();
	_layerManager.add<FeatureLayer>();
	_layerManager.add<EntitySlotLayer>();
	_layerManager.add<EntityPlacementLayer>();
	_layerManager.add<TilePlacementLayer>();
}

GenerationState CelestialBody::generateChunk(drft::ChunkPosition position, entt::registry& registry)
{
	const sf::Vector3i origin = spatial::toTileSpace(position);
	spatial::AABB<int> volume = { origin, ChunkDimensions };

	auto tilePlacementLayer = _layerManager.generate<TilePlacementLayer>(volume);
	if (!tilePlacementLayer.isReady()) return tilePlacementLayer.getState();

	auto entityPlacementLayer = _layerManager.generate<EntityPlacementLayer>(volume);
	if (!entityPlacementLayer.isReady()) return entityPlacementLayer.getState();

	tilePlacementLayer.unwrap().placeTiles(volume, registry);
	entityPlacementLayer.unwrap().placeEntities(volume, registry);

	return GenerationState::Complete;
}

entt::id_type CelestialBody::getSourceId()
{
	return entt::hashed_string{_name.c_str()};
}

void CelestialBody::tick()
{
}

void CelestialBody::generateInit(entt::registry& registry)
{
	// Generate the starting state of this source
	
	LOG_MSG("Generating {}", this->_name);
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
			for (auto&& [layerId, typeAndParams] : pack.getLayers())
			{
				if (auto layer = _layerManager.add<ICreateFromJson>(typeAndParams.layerTypeId, layerId))
				{
					layer->createFromJson(typeAndParams.json);
				}
				else
				{
					LOG_ERROR("Layer {} could not be converted to a ICreateFromJson layer", layerId);
				}
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
		_layerManager.setEntityPack(_entityPacks);
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
		_layerManager.add<BiomeLayer>()->createFromJson(json);
	}
	if (json.HasMember("bodies"))
	{
		_celestialBodies.reserve(json["bodies"].GetArray().Size());
		for (auto&& body : json["bodies"].GetArray())
		{
			_celestialBodies.emplace_back(_registries);
			_celestialBodies.back().createFromJson(body);
		}
	}
}
