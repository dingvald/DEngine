#pragma once

namespace drft::gen
{
	class WorldGenerator;
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
		VirtualChunk(sf::Vector2i coordinate)
			: _coordinate(coordinate)
			, _state(ChunkState::None) {}

		void setState(ChunkState state);
		ChunkState getState() const;

		ioStatus build(entt::registry& reg);
		ioStatus save(entt::registry& reg, const std::filesystem::path& filename) const;
		ioStatus load(entt::registry& reg, const std::filesystem::path& filename) const;

		ioStatus asyncLoad(entt::registry& reg, const std::filesystem::path& filename);
		ioStatus asyncSave(entt::registry& reg, const std::filesystem::path& filename);

		std::string toString() const;

	private:
		void setFuture(std::shared_future<bool> future);
		const std::shared_future<bool>& getFuture() const;

		bool saveChunkToFile(const std::filesystem::path& filename) const;
		bool loadChunkFromFile(const std::filesystem::path& filename);

	private:
		sf::Vector2i _coordinate;
		ChunkState _state = ChunkState::None;
		std::shared_future<bool> _future;
		entt::registry _asyncRegistry;
	};

}

