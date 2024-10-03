#pragma once
#include "Systems/System.h"

#include <Spatial/ChunkPosition.h>
#include "Spatial/VirtualChunk.h"
#include "Utility/stdHashing.h"

namespace drft::system
{
	struct CameraInfo;
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

		virtual void onUpdate(const float dt) override;
		virtual void shutdown() override;

	private:
		void updateChunkStates(const CameraInfo& camera);
		void cleanUpChunks();
		void processBuildQueue();
		void processLoadQueue();
		void processSaveQueue();
		void loadOrBuildChunk(ChunkPosition position, spatial::VirtualChunk& chunk);
		std::filesystem::path buildChunkFilename(const spatial::VirtualChunk& chunk) const;

		bool isWithinChunkSaveDisk(sf::Vector3i chunkPosition, sf::Vector3i centerPosition) const;

	private:
		std::unordered_map<ChunkPosition, spatial::VirtualChunk> _chunks;

		std::queue<ChunkPosition> _toBuild;
		std::queue<ChunkPosition> _toLoad;
		std::queue<ChunkPosition> _toSave;
		std::vector<ChunkPosition> _toDelete;
	};
}



