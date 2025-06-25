#pragma once
#include <CelestialBody/CelestialBody.h>
#include <Cereal/external/rapidjson/document.h>

#include <EnTT/entt.h>

#include <JSON/ICreateFromJson.h>

#include <ProcGen/IChunkDataSource.h>
#include <ProcGen/IChunkDataSourceProvider.h>

#include <vector>

struct GenerationRegistries;

class SolarSystem : public ICreateFromJson, public IChunkDataSourceProvider
{
public:
	SolarSystem(const GenerationRegistries& registries);
	SolarSystem(const SolarSystem&) = delete;
	SolarSystem& operator=(const SolarSystem&) = delete;

	void createFromJson(const rapidjson::Value& json) override;

	IChunkDataSource* tryGetDataSource(entt::id_type sourceId) override;

	void tick();

private:
	std::vector<CelestialBody> _celestialBodies;
	const GenerationRegistries& _registries;
};