#pragma once

namespace drft::system
{
	enum class WhereToLook
	{
		Containers,
		Bodies,
		Both
	};

	entt::entity findItemOwner(entt::registry& registry, unsigned long itemID, WhereToLook whereToLook);
}