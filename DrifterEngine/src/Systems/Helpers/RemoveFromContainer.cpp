#include "pch.h"
#include "RemoveFromContainer.h"
#include "Components/Components.h"

void drft::system::removeFromContainer(entt::registry& registry, entt::entity container, entt::entity item, bool destroyAfterRemoval)
{
	auto hasContainer = registry.all_of<component::Container>(container);
	auto itemComp = registry.try_get<component::Item>(item);
	if (hasContainer && itemComp)
	{
		registry.patch<component::Container>(container,
			[itemComp](component::Container& cont)
			{
				cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), itemComp->id), cont.contents.end());
			});
	}

	if (destroyAfterRemoval)
	{
		registry.destroy(item);
	}
	else
	{
		auto& position = registry.get<component::Position>(container);
		registry.emplace_or_replace<component::Position>(item, position);
	}
	
}

void drft::system::removeFromContainer(entt::registry& registry, component::Container& container, component::Item& item, bool destroyAfterRemoval)
{
	registry.patch<component::Container>(entt::to_entity(registry, container),
		[item](component::Container& cont)
		{
			cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), item.id), cont.contents.end());
		});
	if (destroyAfterRemoval)
	{
		registry.destroy(entt::to_entity(registry, item));
	}
	else
	{
		auto& position = registry.get<component::Position>(entt::to_entity(registry, container));
		registry.emplace_or_replace<component::Position>(entt::to_entity(registry, item), position);
	}
}
