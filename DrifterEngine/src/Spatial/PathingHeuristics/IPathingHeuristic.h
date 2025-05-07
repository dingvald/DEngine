#pragma once
#include <vector>

namespace drft::spatial
{
	using EntityList = std::vector<entt::entity>;

	class IPathingHeuristic
	{
	public:
		virtual int evaluate(const EntityList& entities) const = 0;
	};
}