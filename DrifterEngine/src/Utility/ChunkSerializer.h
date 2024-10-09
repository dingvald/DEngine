#pragma once
#include <Spatial/ChunkPosition.h>
#include <Utility/VectorBuffer.h>

namespace drft
{
	class ChunkSerializer
	{
	public:
		void save(ChunkPosition position, entt::registry& registry);
		void load(ChunkPosition position, entt::registry& registry);

	private:
		VectorBuffer<char> _sharedBuffer = { 4096 };
	};
}


