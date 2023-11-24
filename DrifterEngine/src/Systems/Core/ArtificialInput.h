#pragma once
#include "Systems/System.h"
#include "Systems/HelperClasses/StateMachine.h"
#include "Systems/HelperClasses/AIStates.h"
#include "GOAP/Actions/AiActionTypes.h"
#include "GOAP/Goal.h"
#include "GOAP/ISensor.h"

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
		bool inSightRange(sf::Vector2i position, const component::AI&) const;
		void moveToTarget(entt::handle entity, sf::Vector2i targetPosition) const;
		void pathToTarget(entt::handle, sf::Vector2i targetPosition) const;
		void clearPathCache(entt::entity entity) const;

		entt::handle getHandle(component::AI& ai);
		entt::const_handle getHandle(const component::AI& ai) const;

		void registerSensor(std::unique_ptr<goap::ISensor> sensor);
		void senseWorldState(component::AI& ai) const;
		std::deque<goap::AiAction> generatePlan(const component::AI& ai, const goap::Goal& exclude = {}) const;
		int calculateGoalValue(const goap::Goal& goal, const component::AI& ai) const;
		std::queue<goap::Goal> prioritizeGoals(const component::AI& ai, const goap::Goal& exclude = {}) const;
		std::unordered_set<goap::AiAction> getAiActions(const component::AI& ai) const;

		// States
		void aiThink(component::AI& ai);
		void aiMoveTo(component::AI& ai);
		void aiPerformAction(component::AI& ai);

	private:
		std::vector<std::unique_ptr<goap::ISensor>> _sensors;
		using aStarPath = std::deque<sf::Vector2i>;
		mutable std::unordered_map<entt::entity, aStarPath> _cachedPaths;
	};
}

