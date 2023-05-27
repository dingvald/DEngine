#pragma once

namespace component
{
	struct Container;
	struct Item;
}

namespace drft::system
{
	void addToContainer(entt::registry& registry, entt::entity container, entt::entity item);
	void addToContainer(entt::registry& registry, component::Container& container, component::Item& item);
}