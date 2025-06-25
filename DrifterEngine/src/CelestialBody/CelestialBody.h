#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <ProcGen/IChunkDataSource.h>
#include <ProcGen/IChunkDataSourceProvider.h>
#include <JSON/ICreateFromJson.h>
#include <Utility/Math.h>
#include <ProcGen/EntityPack/EntityPack.h>
#include <ProcGen/GenerationRegistries.h>

class CelestialBody : public IChunkDataSource, public IChunkDataSourceProvider, public ICreateFromJson
{
public:
	CelestialBody(const GenerationRegistries& registries);
	~CelestialBody() = default;

	CelestialBody(const CelestialBody&) = delete;
	CelestialBody& operator=(const CelestialBody&) = delete;

	CelestialBody(CelestialBody&&) = default;
	CelestialBody& operator=(CelestialBody&&) = default;

	void createFromJson(const rapidjson::Value& json) override;

	entt::id_type getSourceId() override;
	IChunkDataSource* tryGetDataSource(entt::id_type sourceId) override;
	drft::GenerationLayerManager& getGenerationLayers() override;

	void tick();

private:
	std::string _name;
	drft::math::Range<int> _sizeRange;
	drft::math::Range<float> _distanceRange;
	sf::Vector2f _position;
	std::vector<CelestialBody> _celestialBodies;
	EntityPack _entityPacks;
	const GenerationRegistries& _registries;
	drft::GenerationLayerManager _layers;
};