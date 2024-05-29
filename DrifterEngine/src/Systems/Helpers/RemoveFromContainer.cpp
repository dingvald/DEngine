#include "pch.h"
#include "RemoveFromContainer.h"
#include "Components/PositionComponent.h"
#include "Components/ContainerComponent.h"
#include "Components/ItemComponent.h"

void drft::system::removeFromContainer(entt::registry& registry, entt::entity container, entt::entity item, bool destroyAfterRemoval)
{
	auto hasContainer = registry.all_of<ContainerComponent>(container);
	auto itemComp = registry.try_get<ItemComponent>(item);
	if (hasContainer && itemComp)
	{
		registry.patch<ContainerComponent>(container,
			[itemComp](ContainerComponent& cont)
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
		auto& position = registry.get<PositionComponent>(container);
		registry.emplace_or_replace<PositionComponent>(item, position);
	}
	
}

void drft::system::removeFromContainer(entt::registry& registry, ContainerComponent& container, ItemComponent& item, bool destroyAfterRemoval)
{
	registry.patch<ContainerComponent>(entt::to_entity(registry, container),
		[item](ContainerComponent& cont)
		{
			cont.contents.erase(std::remove(cont.contents.begin(), cont.contents.end(), item.id), cont.contents.end());
		});
	if (destroyAfterRemoval)
	{
		registry.destroy(entt::to_entity(registry, item));
	}
	else
	{
		auto& position = registry.get<PositionComponent>(entt::to_entity(registry, container));
		registry.emplace_or_replace<PositionComponent>(entt::to_entity(registry, item), position);
	}
}
