#include "pch.h"
#include "AddToContainer.h"
#include "Components/ContainerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ItemComponent.h"

void drft::system::addToContainer(entt::registry& registry, entt::entity container, entt::entity item)
{
	auto hasContainer = registry.all_of<ContainerComponent>(container);
	auto itemComp = registry.try_get<ItemComponent>(item);

	if (!hasContainer || !itemComp) return;

	registry.remove<PositionComponent>(item);
	registry.patch<ContainerComponent>(container,
		[itemComp](ContainerComponent& cont)
		{
			cont.contents.push_back(itemComp->id);
		});
}

void drft::system::addToContainer(entt::registry& registry, ContainerComponent& container, ItemComponent& item)
{
	registry.remove<PositionComponent>(entt::to_entity(registry, item));
	registry.patch<ContainerComponent>(entt::to_entity(registry, container),
		[&item](ContainerComponent& cont)
		{
			cont.contents.push_back(item.id);
		});
}
