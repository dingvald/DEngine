#pragma once
#include <Spatial/PathingHeuristics/IPathingHeuristic.h>

namespace drft::spatial
{
	class PhysicalBlockingHeuristic : public IPathingHeuristic
	{
	public:
		PhysicalBlockingHeuristic(entt::registry& registry);
		int evaluate(const EntityList& entities) const override;

	private:
		entt::registry& _registry;
	};
}
