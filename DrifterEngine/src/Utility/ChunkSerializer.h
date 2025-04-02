#pragma once
#include <Spatial/ChunkPosition.h>
#include <Utility/RegionFile.h>
#include <Utility/stdHashing.h> 

namespace drft
{
	struct SourceChunkPositionPair
	{
		entt::id_type sourceId;
		ChunkPosition position;

		bool operator==(const SourceChunkPositionPair& other) const
		{
			return (sourceId == other.sourceId)
				&& (position == other.position);
		}

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(sourceId, position);
		}
	};
}



template<>
struct std::hash<drft::SourceChunkPositionPair>
{
	size_t operator() (const drft::SourceChunkPositionPair& pos) const noexcept
	{
		size_t seed = pos.sourceId;
		drft::ChunkPosition positionCopy = pos.position;
		hash_combine<drft::ChunkPosition>(seed, positionCopy);
		return seed;
	}
};

namespace drft
{
	class ChunkSerializer
	{
	public:
		ChunkSerializer();
		~ChunkSerializer();

		bool isSerialized(SourceChunkPositionPair position) const;

		std::future<void> queueForSave(SourceChunkPositionPair sourcePositionPair, entt::registry& registry);
		std::future<void> queueForLoad(SourceChunkPositionPair sourcePositionPair, entt::registry& registry);

	private:
		void serializationThread();

		void syncSaveList();
		void syncLoadList();

		void processSaveList();
		void processLoadList();

		void saveSerializedChunkList();
		void loadSerializedChunkList();

		std::filesystem::path getRegionFilePath(SourceChunkPositionPair sourcePositionPair) const;

		static std::vector<char> serializeAndCompressRegistry(const entt::registry& registry);
		static void decompressAndDeserializeRegistry(const std::vector<char>& compressed, entt::registry& registry);

	private:
		struct ChunkRegistryPair
		{
			SourceChunkPositionPair position;
			entt::registry& registry;
		};
		std::unordered_set<SourceChunkPositionPair> _serializedChunks;

		std::unordered_map<std::filesystem::path, RegionFile> _regionFiles;

		std::thread _serializationThread;
		std::atomic_bool _shouldShutdown = false;

		std::mutex _saveQueueLock;
		std::mutex _loadQueueLock;
		std::vector<ChunkRegistryPair> _saveQueue;
		std::vector<ChunkRegistryPair> _loadQueue;

		std::mutex _savePromiseLock;
		std::mutex _loadPromiseLock;
		std::unordered_map<SourceChunkPositionPair, std::promise<void>> _savePromises;
		std::unordered_map<SourceChunkPositionPair, std::promise<void>> _loadPromises;

		std::unordered_map<std::filesystem::path, std::vector<ChunkRegistryPair>> _saveList;
		std::unordered_map<std::filesystem::path, std::vector<ChunkRegistryPair>> _loadList;
	};
}


