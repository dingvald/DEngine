#pragma once
#include "Systems/System.h"
#include "Systems/HelperClasses/StateMachine.h"
#include "Systems/HelperClasses/AIStates.h"
#include "GOAP/Actions/AiActionTypes.h"

namespace component
{
	struct AI;
}

namespace drft::goap
{
	class WorldState;
}

namespace drft::system
{
	class ArtificialInput : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		bool inSightRange(sf::Vector2i position, const component::AI&) const;
		void randomMove(entt::handle entity) const;
		void moveToTarget(entt::handle entity, sf::Vector2i targetPosition) const;
		void pathToTarget(entt::handle, sf::Vector2i targetPosition) const;
		void clearPathCache(entt::entity entity) const;

		entt::handle getHandle(component::AI& ai);
		entt::const_handle getHandle(const component::AI& ai) const;

		void generatePlan(component::AI& ai) const;
		std::stack<std::reference_wrapper<const goap::WorldState>> prioritizeGoals(const component::AI& ai) const;
		std::unordered_set<goap::AiAction> getAiActions(const component::AI& ai) const;

		// States
		void aiThink(component::AI& ai);
		void aiMoveTo(component::AI& ai);
		void aiPerformAction(component::AI& ai);

	private:
		using aStarPath = std::deque<sf::Vector2i>;
		mutable std::unordered_map<entt::entity, aStarPath> _cachedPaths;
	};
}

