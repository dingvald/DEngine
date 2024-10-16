#pragma once
#include <Spatial/ChunkPosition.h>
#include <Utility/RegionFile.h>

namespace drft
{
	class ChunkSerializer
	{
	public:
		ChunkSerializer();
		~ChunkSerializer();

		bool isSerialized(ChunkPosition position) const;

		std::future<void> queueForSave(ChunkPosition position, entt::registry& registry);
		std::future<void> queueForLoad(ChunkPosition position, entt::registry& registry);

		static std::vector<char> serializeAndCompressRegistry(const entt::registry& registry);
		static void decompressAndDeserializeRegistry(const std::vector<char>& compressed, entt::registry& registry);

	private:
		void serializationThread();

		void syncSaveList();
		void syncLoadList();

		void processSaveList();
		void processLoadList();

		void saveSerializedChunkList();
		void loadSerializedChunkList();

		std::filesystem::path getRegionFilePath(ChunkPosition position) const;

	private:
		struct ChunkRegistryPair
		{
			ChunkPosition position;
			entt::registry& registry;
		};
		std::unordered_set<ChunkPosition> _serializedChunks;

		std::unordered_map<std::filesystem::path, RegionFile> _regionFiles;

		std::thread _serializationThread;
		std::atomic_bool _shouldShutdown = false;

		std::mutex _saveQueueLock;
		std::mutex _loadQueueLock;
		std::vector<ChunkRegistryPair> _saveQueue;
		std::vector<ChunkRegistryPair> _loadQueue;

		std::mutex _savePromiseLock;
		std::mutex _loadPromiseLock;
		std::unordered_map<ChunkPosition, std::promise<void>> _savePromises;
		std::unordered_map<ChunkPosition, std::promise<void>> _loadPromises;

		std::unordered_map<std::filesystem::path, std::vector<ChunkRegistryPair>> _saveList;
		std::unordered_map<std::filesystem::path, std::vector<ChunkRegistryPair>> _loadList;
	};
}


