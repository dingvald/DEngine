#include "pch.h"
#include "SolarSystem.h"
#include <EnTT/entt.h>
#include <ProcGen/IChunkDataSource.h>
#include <ProcGen/GenerationRegistries.h>

SolarSystem::SolarSystem(const GenerationRegistries& registries)
	: _registries(registries)
{
}

void SolarSystem::createFromJson(const rapidjson::Value& json)
{
	if (!json.HasMember("bodies")) return;

	for (auto&& body : json["bodies"].GetArray())
	{
		CelestialBody newBody{_registries};
		newBody.createFromJson(body);
		_celestialBodies.emplace_back(std::move(newBody));
	}
}

void SolarSystem::tick()
{
	for (auto&& celestialBodies :_celestialBodies)
	{
		celestialBodies.tick();
	}
}

IChunkDataSource* SolarSystem::tryGetDataSource(entt::id_type sourceId)
{
	for (auto&& celestialBody : _celestialBodies)
	{
		if (celestialBody.getSourceId() == sourceId) return &celestialBody;
		if (auto generator = celestialBody.tryGetDataSource(sourceId))
		{
			return generator;
		}
	}
	return nullptr;
}
