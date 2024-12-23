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
		void update() override;

		void onMoveActionAdded(entt::registry& registry, entt::entity entity) const;
		void processMoveAction(entt::entity entity, MoveAction& action) const;
		void move(entt::handle entity, sf::Vector2i direction) const;
	};
}