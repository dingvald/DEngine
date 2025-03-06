#pragma once
#include "Systems/System.h"

#include <Spatial/ChunkPosition.h>
#include "Spatial/VirtualChunk.h"
#include "Utility/stdHashing.h"
#include <Utility/ChunkSerializer.h>

namespace drft::system
{
	struct CameraHandle;
	namespace
	{
		enum ProcessType
		{
			BUILD,
			SAVE,
			LOAD
		};
	}
	
	class ChunkManager : public System
	{
	public:
		using System::System;

		virtual void update() override;
		virtual void shutdown() override;

	private:
		void updateChunkStates(const CameraHandle& camera);
		void cleanUpChunks();
		void processBuildQueue();
		void processLoadQueue();
		void processSaveQueue();
		void loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk);

		bool isWithinChunkSaveDisk(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const;

	private:
		std::unordered_map<ChunkPosition, spatial::VirtualChunk> _chunks;
		ChunkSerializer _serializer;

		std::vector<ChunkPosition> _toBuild;
		std::vector<ChunkPosition> _toLoad;
		std::vector<ChunkPosition> _toSave;
		std::vector<ChunkPosition> _toDelete;
	};
}



