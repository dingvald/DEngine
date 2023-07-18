#pragma once

namespace component
{
	struct Item;
	struct Container;
}

namespace drft::system
{
	bool containerHasItem(entt::registry& registry, entt::entity entity, std::string itemName);
}