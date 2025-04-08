#pragma once
#include "Snapshot/Reflection.h"
#include <Spatial/TilePosition.h>

struct ChunkSourceTrackerComponent
{
	entt::id_type sourceId;
	drft::TilePosition position;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "chunk_source_tracker";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ChunkSourceTrackerComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ChunkSourceTrackerComponent& tracker)
	{
		archive(tracker.sourceId, tracker.position);
	}
}