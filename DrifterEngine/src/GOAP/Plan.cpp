#include "pch.h"
#include "Plan.h"
#include "ActionRegistry.h"
#include "GoalRegistry.h"


std::optional<std::deque<drft::goap::AiAction>> drft::goap::plan(const goap::WorldState& blackboard, const std::unordered_set<AiAction>& actions, const WorldState& goal)
{
	struct Node
	{
		const WorldState* worldState;
		AiAction cameFrom;
		int cost = 0;
		int distance = 0;
		bool operator<(const Node& rhs) const
		{
			return cost < rhs.cost;
		}
	};

	std::set<Node> openSet;
	std::unordered_set<AiAction> closedSet;
	std::unordered_map<AiAction, Node> cameFrom;

	auto constructPlan = [](const std::unordered_map<AiAction, Node>& cameFrom, const Node& current) {
		std::deque<AiAction> result;
		Node currentNode = current;
		while (currentNode.cameFrom != AiAction::NULL_TYPE)
		{
			result.emplace_front(currentNode.cameFrom);
			currentNode = cameFrom.at(currentNode.cameFrom);
		}
		return result;
	};

	Node startNode = { &blackboard, AiAction::NULL_TYPE, 0, 0};
	openSet.insert(startNode);
	cameFrom.emplace(startNode.cameFrom, startNode);

	while (!openSet.empty())
	{
		auto& currentNode = *(openSet.begin());
		if (currentNode.worldState->isSupersetOf(goal))
		{
			return constructPlan(cameFrom, currentNode);
		}
		openSet.erase(openSet.begin());
		closedSet.emplace(currentNode.cameFrom);

		for (auto& actionType : actions)
		{
			if (closedSet.contains(actionType)) continue;
			const auto& action = goap::ActionRegistry::get(actionType);
			if (action.preconditions().isSubsetOf(*currentNode.worldState))
			{
				const int distanceSoFar = currentNode.distance + 1;
				const int distanceFromTarget = action.effects().distance(goal);
				const int cost = distanceSoFar + distanceFromTarget + action.cost();

				Node neighbor = Node({ &action.effects(), actionType, distanceSoFar, cost });

				auto inOpenSet = openSet.find(neighbor);
				if (inOpenSet == openSet.end())
				{
					openSet.emplace(neighbor);
				}
				else
				{
					if (inOpenSet->distance > distanceSoFar)
					{
						openSet.erase(inOpenSet);
						openSet.emplace(neighbor);
					}
				}
			}
		}
	}

	return std::nullopt;
}
