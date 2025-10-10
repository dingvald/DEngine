#include "pch.h"
#include "CelestialBody.h"

#include <Cereal/external/rapidjson/document.h>
#include <EnTT/entt.h>
#include <Generation/Layers/BiomeLayer.h>
#include <Generation/Layers/StructuresLayer.h>

#include <Spatial/AABB.h>

#include <Generation/GenerationRegistries.h>
#include <Generation/EntityPack/EntityPack.h>
#include <Generation/LayerPack/LayerPack.h>
#include <Generation/LayeredProcGen/LayeredProcGenBinder.h>

#include <Utility/StandardLogger.h>

using namespace drft;
using namespace entt::literals;

CelestialBody::CelestialBody(const GenerationRegistries& registries)
	: _registries(registries)
	, _layers(registries)
{
	LayeredProcGenBinder::bindLayers(_layers);
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
	if (json.HasMember("structures"))
	{
		_layers.add<StructuresLayer>()->createFromJson(json);
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
