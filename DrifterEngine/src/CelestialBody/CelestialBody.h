#pragma once
#include <ProcGen/IChunkGenerator.h>
#include <ProcGen/IChunkGeneratorProvider.h>
#include <JSON/ICreateFromJson.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Utility/Math.h>

struct RegistriesProvider;

class CelestialBody : public IChunkGenerator, public IChunkGeneratorProvider, public ICreateFromJson
{
public:
	CelestialBody(const RegistriesProvider& registries);
	~CelestialBody() = default;

	CelestialBody(const CelestialBody&) = delete;
	CelestialBody& operator=(const CelestialBody&) = delete;

	CelestialBody(CelestialBody&&) = default;
	CelestialBody& operator=(CelestialBody&&) = default;

	void createFromJson(const rapidjson::Value& json) override;

	GenerationState generateChunk(drft::ChunkPosition position, entt::registry& registry) override;
	entt::id_type getSourceId() override;
	IChunkGenerator* tryGetGenerator(entt::id_type sourceId) override;

	void tick();

private:
	std::string _name;
	entt::id_type _generator;
	drft::math::Range<int> _sizeRange;
	drft::math::Range<float> _distanceRange;
	sf::Vector2f _position;
	drft::GenerationLayerManager _layerManager;
	std::vector<CelestialBody> _celestialBodies;
	const RegistriesProvider& _registries;
};