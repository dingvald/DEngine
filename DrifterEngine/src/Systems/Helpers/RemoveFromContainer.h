#pragma once

namespace component
{
	struct Container;
	struct Item;
}

namespace drft::system
{
	void removeFromContainer(entt::registry& registry, entt::entity container, entt::entity item, bool destroyAfterRemoval = false);
	void removeFromContainer(entt::registry& registry, component::Container& container, component::Item& item, bool destroyAfterRemoval = false);
}