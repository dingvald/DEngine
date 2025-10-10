#pragma once
#include <Spatial/ChunkPosition.h>
#include <EnTT/entt.h>

namespace drft
{
	class GenerationLayerManager;
}

class IChunkDataSource
{
public:
	virtual drft::GenerationLayerManager& getGenerationLayers() = 0;
	virtual entt::id_type getSourceId() = 0;
};