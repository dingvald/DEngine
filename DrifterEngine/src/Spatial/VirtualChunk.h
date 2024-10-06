#pragma once
#include <Spatial/ChunkPosition.h>

namespace drft::gen
{
	class WorldGenerator;
}

namespace BS
{
	class thread_pool;
}

namespace drft::spatial
{
	enum class ioStatus
	{
		Busy,
		Done
	};

	enum class ChunkState
	{
		None,
		Active,
		Built,
		ToBuild,
		Building,
		Saved,
		ToSave,
		Saving,
		Loaded,
		ToLoad,
		Loading
	};

	struct VirtualChunk
	{
	public:
		VirtualChunk(ChunkPosition coordinate)
			: _coordinate(coordinate)
			, _state(ChunkState::None) {}

		void setState(ChunkState state);
		ChunkState getState() const;
		ChunkPosition getPosition() const;

		ioStatus build(entt::registry& reg);
		ioStatus save(entt::registry& reg, const std::filesystem::path& filename) const;
		ioStatus load(entt::registry& reg, const std::filesystem::path& filename) const;

		ioStatus asyncLoad(entt::registry& reg, BS::thread_pool& threadPool, const std::filesystem::path& filename);
		ioStatus asyncSave(entt::registry& reg, BS::thread_pool& threadPool, const std::filesystem::path& filename);

		std::string toString() const;

	private:
		void setFuture(std::shared_future<void> future);
		const std::shared_future<void>& getFuture() const;

		bool saveChunkToFile(const std::filesystem::path& filename) const;
		bool loadChunkFromFile(const std::filesystem::path& filename);

	private:
		ChunkPosition _coordinate;
		ChunkState _state = ChunkState::None;
		std::shared_future<void> _future;
		entt::registry _asyncRegistry;
	};

}

