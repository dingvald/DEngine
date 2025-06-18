#pragma once
#include <ProcGen/IChunkGenerator.h>
#include <ProcGen/IChunkGeneratorProvider.h>
#include <JSON/ICreateFromJson.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <Utility/Math.h>
#include <ProcGen/EntityPack/EntityPack.h>
#include <ProcGen/GenerationRegistries.h>

class CelestialBody : public IChunkGenerator, public IChunkGeneratorProvider, public ICreateFromJson
{
public:
	CelestialBody(const GenerationRegistries& registries);
	~CelestialBody() = default;

	CelestialBody(const CelestialBody&) = delete;
	CelestialBody& operator=(const CelestialBody&) = delete;

	CelestialBody(CelestialBody&&) = default;
	CelestialBody& operator=(CelestialBody&&) = default;

	void createFromJson(const rapidjson::Value& json) override;

	void generateInit(entt::registry& registry) override;
	GenerationState generateChunk(drft::ChunkPosition position, entt::registry& registry) override;
	entt::id_type getSourceId() override;
	void setGenerationMode(GenerationMode mode) override;
	IChunkGenerator* tryGetGenerator(entt::id_type sourceId) override;

	void tick();

private:
	std::string _name;
	drft::math::Range<int> _sizeRange;
	drft::math::Range<float> _distanceRange;
	sf::Vector2f _position;
	drft::GenerationLayerManager _layerManager;
	std::vector<CelestialBody> _celestialBodies;
	EntityPack _entityPacks;
	const GenerationRegistries& _registries;
};