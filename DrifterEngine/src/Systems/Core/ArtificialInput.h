#pragma once
#include "Systems/System.h"
#include "Systems/HelperClasses/AIStates.h"
#include "GOAP/Actions/AiActionTypes.h"
#include "GOAP/IGoal.h"
#include "GOAP/SensorRunner.h"


namespace drft::goap
{
	using Plan = std::deque<AiAction>;
}

struct AIComponent;

namespace drft::events
{
	struct TurnEndEvent;
}

namespace drft::system
{
	class ArtificialInput : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		using GoalName = std::string;
		bool inSightRange(sf::Vector2i position, const AIComponent&) const;
		void moveToTarget(entt::handle entity, sf::Vector2i targetPosition) const;
		void pathToTarget(entt::handle, sf::Vector2i targetPosition) const;
		void clearPathCache(entt::entity entity) const;

		entt::handle getHandle(const AIComponent& ai) const;

		void onTurnEndEvent(const events::TurnEndEvent& ev);
		void senseWorldState(AIComponent& ai);
		std::deque<goap::AiAction> generatePlan(AIComponent& ai, std::deque<GoalName>& goals) const;
		bool isPlanValid(const goap::WorldState& worldState, const goap::Plan& plan) const;
		std::deque<GoalName> prioritizeGoals(const AIComponent& ai) const;
		std::unordered_set<goap::AiAction> getAiActions(const AIComponent& ai) const;
		void setNextState(AIComponent& ai, AIState state) const;
		void executeStateNow(AIComponent& ai, AIState state) const;

		// States
		void aiThink(AIComponent& ai) const;
		void aiMoveTo(AIComponent& ai) const;
		void aiPerformAction(AIComponent& ai) const;

	private:
		goap::SensorRunner _sensorySystem;
		using aStarPath = std::deque<sf::Vector2i>;
		mutable std::unordered_map<entt::entity, aStarPath> _cachedPaths;
	};
}

