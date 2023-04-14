#include "pch.h"
#include "CopyEntity.h"
#include "Components/Components.h"
#include "Components/Meta.h"

using namespace entt::literals;

void drft::util::copyEntity(entt::entity to, entt::entity from, entt::registry& registry)
{
	auto& prototypeStorage = registry.view<component::Prototype>().storage();

	for (auto [id, storage] : registry.storage())
	{
		if (storage.contains(from) && !(storage.type() == prototypeStorage.type()))
		{
			if (storage.contains(to))
			{
				storage.erase(to);
			}
			storage.emplace(to, storage.get(from));
		}
	}
}

void drft::util::copyEntity(entt::entity to, entt::entity from, entt::registry& toRegistry, const entt::registry& fromRegistry)
{
	auto& prototypeStorage = fromRegistry.view<component::Prototype>().storage();
	for (auto [id, storage] : fromRegistry.storage())
	{
		if (storage.contains(from) && storage.type() != prototypeStorage.type())
		{
			auto toStorage = toRegistry.storage(id);
			if (!toStorage)
			{
				auto meta = entt::resolve(storage.type());
				meta.from_void(storage.get(from));
				auto func = meta.func("emplace"_hs);
				if (func)
				{
					func.invoke({}, entt::forward_as_meta(toRegistry), to);
				}
			}
			else
			{
				toStorage->emplace(to, storage.get(from));
			}
		}
	}
}

void drft::util::copyEntities(const std::vector<entt::entity>& entities, entt::registry& toRegistry, const entt::registry& fromRegistry)
{
	for (auto e : entities)
	{
		auto toEntity = toRegistry.create();
		copyEntity(toEntity, e, toRegistry, fromRegistry);
	}
}

void drft::util::copyEntities(entt::registry& toRegistry, entt::registry& fromRegistry)
{
	fromRegistry.each([&](auto from_e)
		{
			auto to_e = toRegistry.create();
			copyEntity(to_e, from_e, toRegistry, fromRegistry);
		}
	);
}

