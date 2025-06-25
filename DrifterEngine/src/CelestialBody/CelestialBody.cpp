#include "pch.h"
#include "CelestialBody.h"

#include <Cereal/external/rapidjson/document.h>
#include <EnTT/entt.h>
#include <ProcGen/Layers/BiomeLayer.h>

#include <Spatial/AABB.h>
#include <Spatial/ChunkPosition.h>

#include <ProcGen/GenerationRegistries.h>
#include <ProcGen/EntityPack/EntityPack.h>
#include <ProcGen/LayerPack/LayerPack.h>

#include <Utility/StandardLogger.h>

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

#pragma optimize("", off)

using namespace drft;
using namespace entt::literals;

CelestialBody::CelestialBody(const GenerationRegistries& registries)
	: _registries(registries)
	, _layers(registries)
{
	using namespace entt::literals;
	// register layer types that can have multiple instances created using add
	_layers.registerType<drft::RandomLayer>("random"_hs);
	_layers.registerType<drft::PerlinNoiseLayer>("perlin"_hs);
	_layers.registerType<drft::LloydRelaxedLayer>("relaxed_points"_hs);
	_layers.registerType<drft::FillLayer>("fill"_hs);
	_layers.registerType<drft::DepthLayer>("depth"_hs);

	// Add generic layers that all generators can use
	_layers.add<drft::RandomLayer>();
	_layers.add<drft::JitteredGridLayer>();
	_layers.add<drft::VoronoiLayer>();
	_layers.add<drft::FeatureLayer>();
	_layers.add<drft::EntitySlotLayer>();
	_layers.add<drft::EntityPlacementLayer>();
	_layers.add<drft::TilePlacementLayer>();
}

entt::id_type CelestialBody::getSourceId()
{
	return entt::hashed_string{_name.c_str()};
}

IChunkDataSource* CelestialBody::tryGetDataSource(entt::id_type sourceId)
{
	for (auto&& child : _celestialBodies)
	{
		if (child.getSourceId() == sourceId) return &child;
		if (auto generator = child.tryGetDataSource(sourceId))
		{
			return generator;
		}
	}
	return nullptr;
}

void CelestialBody::tick()
{
}

drft::GenerationLayerManager& CelestialBody::getGenerationLayers()
{
	return _layers;
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
				if (auto layer = _layers.add<ICreateFromJson>(typeAndParams.layerTypeId, layerId))
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
		_layers.setEntityPack(_entityPacks);
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
		_layers.add<BiomeLayer>()->createFromJson(json);
	}
	if (json.HasMember("bodies"))
	{
		_celestialBodies.reserve(json["bodies"].GetArray().Size());
		for (auto&& body : json["bodies"].GetArray())
		{
			_celestialBodies.emplace_back(_registries).createFromJson(body);
		}
	}
}
