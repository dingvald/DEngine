#include "pch.h"
#include "CopyEntity.h"
#include <Components/ComponentMetaBinder.h>
#include <Utility/StandardLogger.h>

using namespace entt::literals;

void drft::util::copyEntity(entt::entity to, entt::entity from, entt::registry& registry, bool overwrite)
{
	for (auto [id, storage] : registry.storage())
	{
		if (storage.contains(from))
		{
			if (storage.contains(to))
			{
				if (!overwrite) continue;
				storage.erase(to);
			}
			storage.emplace(to, storage.get(from));
		}
	}
}

void drft::util::copyEntity(entt::entity to, entt::entity from, entt::registry& toRegistry, const entt::registry& fromRegistry)
{
	for (auto [id, fromStorage] : fromRegistry.storage())
	{
		if (fromStorage.contains(from))
		{
			auto toStorage = toRegistry.storage(id);
			if (!toStorage)
			{
				const auto meta = entt::resolve(ComponentMetaBinder::cxt(), fromStorage.type());
				if (const auto func = meta.func("emplace"_hs)) [[likely]]
				{
					func.invoke(meta, entt::forward_as_meta(toRegistry), to);
					toStorage = toRegistry.storage(id);
					toStorage->remove(to);
					toStorage->emplace(to, fromStorage.get(from));
				}
				else
				{
					LOG_ERROR("Could not find type {} in component reflection context", fromStorage.type().name());
				}
			}
			else
			{
				toStorage->remove(to);
				toStorage->emplace(to, fromStorage.get(from));
			}
		}
	}
}

void drft::util::copyEntities(const std::vector<entt::entity>& entities, entt::registry& toRegistry, const entt::registry& fromRegistry)
{
	for (auto e : entities)
	{
		entt::entity toEntity = toRegistry.create();
		copyEntity(toEntity, e, toRegistry, fromRegistry);
	}
}

void drft::util::copyEntities(entt::registry& toRegistry, entt::registry& fromRegistry)
{
	fromRegistry.each([&](auto from_e)
		{
			entt::entity to_e = toRegistry.create();
			copyEntity(to_e, from_e, toRegistry, fromRegistry);
		});
}

