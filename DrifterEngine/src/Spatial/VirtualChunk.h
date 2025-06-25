#pragma once
#include <Spatial/ChunkPosition.h>

class ChunkGenerator;

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

		ioStatus asyncBuild(entt::registry& reg, ChunkGenerator& generator);
		ioStatus asyncLoad(entt::registry& reg, ChunkSerializer& serializer);
		ioStatus asyncSave(entt::registry& reg, ChunkSerializer& serializer);

		std::string toString() const;

	private:
		using FutureRegistry = std::future<entt::registry>;
		void setFuture(FutureRegistry&& future);
		FutureRegistry& getFuture();

	private:
		entt::id_type _sourceId;
		ChunkPosition _coordinate;
		ChunkState _state = ChunkState::None;
		FutureRegistry _future;
	};

}

