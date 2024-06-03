#pragma once

struct ContainerComponent;
struct ItemComponent;

namespace drft::system
{
	void addToContainer(entt::registry& registry, entt::entity container, entt::entity item);
	void addToContainer(entt::registry& registry, ContainerComponent& container, ItemComponent& item);
}