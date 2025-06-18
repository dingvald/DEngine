#pragma once
#include <ProcGen/GenerationState.h>
#include <Spatial/ChunkPosition.h>
#include <EnTT/entt.h>
#include <ProcGen/GenerationMode.h>

class IChunkGenerator
{
public:
	// Initialize the generator with any global generation that needs to happen before chunks are generated
	virtual void generateInit(entt::registry& registry) = 0;
	// Generate a chunk at the given position
	virtual GenerationState generateChunk(drft::ChunkPosition position, entt::registry& registry) = 0;
	// Get the ID of the generator
	virtual entt::id_type getSourceId() = 0;
	// Set the mode of generation
	virtual void setGenerationMode(GenerationMode mode) = 0;
};