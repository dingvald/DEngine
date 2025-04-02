#include "pch.h"
#include "SolarSystem.h"
#include <EnTT/entt.h>
#include <ProcGen/IChunkGenerator.h>

void SolarSystem::createFromJson(const rapidjson::Value& json)
{
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
	for (auto&& celestialBodies : _celestialBodies)
	{
		if (celestialBodies.getSourceId() == sourceId) return &celestialBodies;
	}
	return nullptr;
}
