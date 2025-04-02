#pragma once
#include <ProcGen/IChunkGenerator.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>


class CelestialBody : public IChunkGenerator, public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	GenerationState generateChunk(drft::ChunkPosition position, entt::registry& registry) override;
	entt::id_type getSourceId() override;

	void tick();

private:
	sf::Vector2f _position;
	drft::GenerationLayerManager _layerManager;
	std::vector<CelestialBody> _celesialBodies;
};