#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class ArtificialInput : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		entt::entity findTarget(entt::handle entity) const;
		bool hasLineOfSight(sf::Vector2i myPosition, sf::Vector2i targetPosition) const;
		void randomMove(entt::handle entity) const;
		void moveToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const;
		void pathToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const;
		void clearPathCache(entt::entity entity) const;

	private:
		using aStarPath = std::deque<sf::Vector2i>;
		mutable std::unordered_map<entt::entity, aStarPath> _cachedPaths;
	};
}

