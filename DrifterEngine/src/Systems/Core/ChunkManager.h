#pragma once
#include "Systems/System.h"

#include <Spatial/ChunkPosition.h>
#include "Spatial/VirtualChunk.h"
#include "Utility/stdHashing.h"

namespace drft::system
{
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
		virtual void save(cereal::JSONOutputArchive& oarchive);

	private:
		void updateChunkStates(ChunkPosition aroundNewPosition);
		void cleanUpChunks();
		void processBuildQueue();
		void processLoadQueue();
		void processSaveQueue();
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



