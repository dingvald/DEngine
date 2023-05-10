#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class HitEffectSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;

	private:
		void onTakeDamage(entt::registry& registry, entt::entity entity);
		void queueHitEffect(sf::Vector2f position);

	private:
		std::vector<entt::entity> _hitEffects;
	};
}


