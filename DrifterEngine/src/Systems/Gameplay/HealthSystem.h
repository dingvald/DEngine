#include "Systems/System.h"
#include "Events/TurnStartEvent.h"

namespace component::action
{
	struct IncomingDamage;
	struct TakeDamage;
}

namespace drft::system
{
	class HealthSystem : public System
	{
	public:
		using System::System;

		void init() override;
		void onUpdate(const float dt) override;

	private:
		void processIncomingDamage(entt::entity entity, component::action::IncomingDamage& damage) const;
		void processTakeDamage(entt::entity entity, component::action::TakeDamage& damage) const;

		void onTurnStartEvent(events::TurnStartEvent& ev);
		void onHealthComponentAdded(entt::registry& registry, entt::entity entity);
		void onLevelUp(entt::registry& registry, entt::entity entity);
	};
}