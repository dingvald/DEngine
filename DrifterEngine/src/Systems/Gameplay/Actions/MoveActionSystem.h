#pragma	once
#include "Systems/System.h"

namespace drft::system
{
	class MoveActionSystem : public System
	{
	private:
		void init() override;
		void onUpdateEnd() override;

		void onPerformMoveAction(entt::registry& registry, entt::entity entity) const;
		void onTryMoveAction(entt::registry& registry, entt::entity entity) const;
		void onDoMoveAction(entt::registry& registry, entt::entity entity) const;
	};
}