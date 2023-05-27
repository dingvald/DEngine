#include "pch.h"
#include "AddToContainer.h"
#include "Components/Components.h"

void drft::system::addToContainer(entt::registry& registry, entt::entity container, entt::entity item)
{
	auto hasContainer = registry.all_of<component::Container>(container);
	auto itemComp = registry.try_get<component::Item>(item);

	if (!hasContainer || !itemComp) return;

	registry.remove<component::Position>(item);
	registry.patch<component::Container>(container,
		[itemComp](component::Container& cont)
		{
			cont.contents.push_back(itemComp->id);
		});
}

void drft::system::addToContainer(entt::registry& registry, component::Container& container, component::Item& item)
{
	registry.remove<component::Position>(entt::to_entity(registry, item));
	registry.patch<component::Container>(entt::to_entity(registry, container),
		[&item](component::Container& cont)
		{
			cont.contents.push_back(item.id);
		});
}
