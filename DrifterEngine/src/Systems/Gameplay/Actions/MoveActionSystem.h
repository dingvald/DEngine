#pragma	once
#include "Systems/System.h"


struct MoveAction;

namespace drft::system
{
	class MoveActionSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void onUpdateLate(const float dt) override;
		void onUpdateEnd() override;

		void onMoveActionAdded(entt::registry& registry, entt::entity entity) const;
		void processMoveAction(entt::entity entity, MoveAction& action) const;

	private:
		entt::observer _moveActionObserver;
	};
}