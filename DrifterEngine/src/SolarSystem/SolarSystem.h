#pragma once
#include <CelestialBody/CelestialBody.h>
#include <Cereal/external/rapidjson/document.h>

#include <EnTT/entt.h>

#include <JSON/ICreateFromJson.h>

#include <ProcGen/IChunkGenerator.h>
#include <ProcGen/IChunkGeneratorProvider.h>

#include <vector>

class SolarSystem : public ICreateFromJson, public IChunkGeneratorProvider
{
public:
	SolarSystem() = default;
	SolarSystem(const SolarSystem&) = delete;
	SolarSystem& operator=(const SolarSystem&) = delete;

	void createFromJson(const rapidjson::Value& json) override;

	IChunkGenerator* tryGetGenerator(entt::id_type sourceId) override;

	void tick();

private:
	std::vector<CelestialBody> _celestialBodies;
};