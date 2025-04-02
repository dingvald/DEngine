#pragma once
#include <JSON/ICreateFromJson.h>
#include <CelestialBody/CelestialBody.h>
#include <ProcGen/IChunkGeneratorProvider.h>

class SolarSystem : public ICreateFromJson, public IChunkGeneratorProvider
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	IChunkGenerator* get(entt::id_type sourceId) override;

	void tick();

private:
	std::vector<CelestialBody> _celestialBodies;
};