#include "pch.h"
#include "SolarSystem.h"
#include <EnTT/entt.h>
#include <ProcGen/IChunkGenerator.h>

void SolarSystem::createFromJson(const rapidjson::Value& json)
{
	if (!json.HasMember("bodies")) return;

	for (auto&& body : json["bodies"].GetArray())
	{
		CelestialBody newBody;
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

IChunkGenerator* SolarSystem::tryGetGenerator(entt::id_type sourceId)
{
	for (auto&& celestialBody : _celestialBodies)
	{
		if (celestialBody.getSourceId() == sourceId) return &celestialBody;
		if (auto generator = celestialBody.tryGetGenerator(sourceId))
		{
			return generator;
		}
	}
	return nullptr;
}
