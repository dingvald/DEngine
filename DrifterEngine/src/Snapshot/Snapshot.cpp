#include <pch.h>
#include <Components/ComponentMetaBinder.h>
#include <Snapshot/Reflection.h>
#include <Snapshot/Snapshot.h>

namespace snapshot 
{
	void Snapshot::save(OutputArchive archive, const entt::registry& reg)
	{
		entt::snapshot snapshot = { reg };
		for (auto&& [id, meta] : entt::resolve(ComponentMetaBinder::cxt()))
		{
			meta.func(TAKE_COMPONENT_SNAPSHOT_FN_NAME).invoke({}, entt::forward_as_meta(snapshot), archive);
		}
	}

	void SnapshotLoader::load(InputArchive archive, entt::registry& reg)
	{
		entt::snapshot_loader loader = { reg };
		for (auto&& [id, meta] : entt::resolve(ComponentMetaBinder::cxt()))
		{
			meta.func(LOAD_COMPONENT_SNAPSHOT_FN_NAME).invoke({}, entt::forward_as_meta(loader), archive);
		}
	}

} // namespace snapshot