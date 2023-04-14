#pragma once

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
		ioStatus save(entt::registry& reg, const char* filepath);
		ioStatus load(entt::registry& reg, const char* filepath);

		ioStatus asyncLoad(entt::registry& reg, const char* filepath);
		ioStatus asyncSave(entt::registry& reg, const char* filepath);

		std::string toString() const;

	private:
		void setFuture(std::shared_future<bool> future);
		const std::shared_future<bool>& getFuture() const;

		bool saveChunkToFile(const char* filepath) const;
		bool loadChunkFromFile(const char* filepath);

	private:
		sf::Vector2i _coordinate;
		ChunkState _state = ChunkState::None;
		std::shared_future<bool> _future;
		entt::registry _asyncRegistry;
	};

}

