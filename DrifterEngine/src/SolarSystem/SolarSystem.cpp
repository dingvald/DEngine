#include "pch.h"
#include "SolarSystem.h"

void SolarSystem::createFromJson(const rapidjson::Value& json)
{
}

void SolarSystem::tick()
{
	for (auto&& planet :_celestialBodies)
	{
		planet.tick();
	}
}

IChunkGenerator* SolarSystem::get(entt::id_type sourceId)
{
	for (auto&& celestialBodies : _celestialBodies)
	{
		if (celestialBodies.getSourceId() == sourceId) return &celestialBodies;
	}
}
