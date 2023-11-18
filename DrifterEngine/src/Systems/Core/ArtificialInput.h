#pragma once
#include "Systems/System.h"
#include "Systems/HelperClasses/StateMachine.h"
#include "Systems/HelperClasses/AIStates.h"

namespace component
{
	struct AI;
}

namespace drft::system
{
	class ArtificialInput : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		entt::entity findTarget(entt::handle entity, std::function<bool(entt::const_handle, entt::const_handle)> selector) const;
		bool hasLineOfSight(sf::Vector2i myPosition, sf::Vector2i targetPosition) const;
		void randomMove(entt::handle entity) const;
		void moveToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const;
		void pathToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const;
		void clearPathCache(entt::entity entity) const;

		bool isTargetValid(component::AI& ai) const;

		// States
		void aiStandby(component::AI& ai);
		void aiMoveTo(component::AI& ai);
		void aiPerformAction(component::AI& ai);

	private:
		using aStarPath = std::deque<sf::Vector2i>;
		mutable std::unordered_map<entt::entity, aStarPath> _cachedPaths;
	};
}

