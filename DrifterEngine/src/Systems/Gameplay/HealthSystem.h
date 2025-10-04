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
		void start() override;
		void update(const float dt) override;

	private:
		void processTakeDamage(entt::entity entity, component::action::TakeDamage& damage) const;

		void spawnDamageEffects(entt::const_handle entity, float damage) const;

		void onTurnStartEvent(events::TurnStartEvent& ev);
		void onHealthComponentAdded(entt::registry& registry, entt::entity) const;
		void setupHealthComponent(entt::handle entity) const;

	private:
		entt::observer _healthAddedObserver;
	};
}