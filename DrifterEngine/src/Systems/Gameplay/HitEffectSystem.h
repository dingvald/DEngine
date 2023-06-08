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
		void onDie(entt::registry& registry, entt::entity entity);
		void queueEffect(sf::Vector2f position, unsigned int sprite, sf::Color color, std::function<bool(entt::registry&, entt::entity)> onFixedUpdate);
		void removeEffect(entt::entity effect);

	private:
		std::vector<entt::entity> _effects;
		std::unordered_map < entt::entity, std::function<bool(entt::registry&, entt::entity)>> _effectUpdateFunctions;
	};
}


