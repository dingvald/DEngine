#pragma once
#include <Spatial/ChunkPosition.h>

namespace drft::gen
{
	class WorldGenerator;
}

namespace drft
{
	class ChunkSerializer;
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
		VirtualChunk(entt::id_type sourceId, ChunkPosition coordinate)
			: _sourceId(sourceId)
			, _coordinate(coordinate)
			, _state(ChunkState::None) {}

		void setState(ChunkState state);
		ChunkState getState() const;
		ChunkPosition getPosition() const;

		ioStatus build(entt::registry& reg);

		ioStatus asyncLoad(entt::registry& reg, ChunkSerializer& serializer);
		ioStatus asyncSave(entt::registry& reg, ChunkSerializer& serializer);

		std::string toString() const;

	private:
		void setFuture(std::future<void> future);
		const std::future<void>& getFuture() const;

	private:
		entt::id_type _sourceId;
		ChunkPosition _coordinate;
		ChunkState _state = ChunkState::None;
		std::future<void> _future;
		entt::registry _asyncRegistry;
	};

}

