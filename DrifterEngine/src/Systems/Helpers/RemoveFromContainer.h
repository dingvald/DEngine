#pragma once

struct ContainerComponent;
struct ItemComponent;

namespace drft::system
{
	void removeFromContainer(entt::registry& registry, entt::entity container, entt::entity item, bool destroyAfterRemoval = false);
	void removeFromContainer(entt::registry& registry, ContainerComponent& container, ItemComponent& item, bool destroyAfterRemoval = false);
}