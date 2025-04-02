#pragma once
#include <ProcGen/GenerationState.h>
#include <Spatial/ChunkPosition.h>
#include <EnTT/entt.h>

class IChunkGenerator
{
public:
	virtual GenerationState generateChunk(drft::ChunkPosition position, entt::registry& registry) = 0;
	virtual entt::id_type getSourceId() = 0;
};