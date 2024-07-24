#include "Systems/System.h"
#include "Events/TurnStartEvent.h"

namespace drft::system
{
	class HealthSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void onUpdate(const float dt) override;
		void onUpdateEnd() override;

	private:
		void onTurnStartEvent(events::TurnStartEvent& ev);
		void onHealthComponentAdded(entt::registry& registry, entt::entity entity);
		void onLevelUp(entt::registry& registry, entt::entity entity);
	};
}