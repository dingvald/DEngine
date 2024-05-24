#pragma once

#include "Systems/HelperClasses/AIStates.h"
#include "GOAP/WorldState.h"
#include "GOAP/Actions/AiActionTypes.h"
#include "GOAP/SensorTypes.h"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct AIComponent
{
	int sightRange = 0;
	std::unordered_set<std::string> goals;
	std::string currentGoal = {};
	std::deque<drft::goap::AiAction> plan;
	drft::goap::WorldState blackboard;
	AIState state = AIState::Think;

	// Runtime only
	using SurroundingsMemory = std::unordered_map<drft::goap::SensorType, std::unordered_map<entt::entity, int>>;
	SurroundingsMemory surroundings;
	entt::entity target = entt::null;
private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "AI";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<AIComponent, NAME>()
			.prop("serialize"_hs)
			.data<&AIComponent::sightRange>("sightRange"_hs)
			.data<&AIComponent::goals>("goals"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, AIComponent& ai)
	{
		archive(ai.sightRange, ai.goals, ai.currentGoal, ai.plan, ai.blackboard, ai.state);
	}
}
