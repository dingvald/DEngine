#include "pch.h"
#include "Plan.h"
#include "ActionRegistry.h"
#include "GoalRegistry.h"


std::optional<drft::goap::Plan> drft::goap::plan(const goap::WorldState& blackboard, const std::unordered_set<AiAction>& actions, const WorldState& goal)
{
	struct Node
	{
		WorldState worldState;
		AiAction action;
		int cost = 0;
		int distance = 0;
		bool operator<(const Node& rhs) const
		{
			if (cost == rhs.cost)
			{
				return rhs.worldState.contains(worldState);
			}
			return cost < rhs.cost;
		}
	};

	std::set<Node> openSet;
	std::unordered_set<AiAction> closedSet;
	std::unordered_map<AiAction, Node> cameFrom;

	auto constructPlan = [](const std::unordered_map<AiAction, Node>& cameFrom, const Node& current) {
		std::deque<AiAction> result;
		Node currentNode = current;
		while (currentNode.action != AiAction::NULL_TYPE)
		{
			result.push_front(currentNode.action);
			currentNode = cameFrom.at(currentNode.action);
		}
		return result;
	};

	Node startNode = { blackboard, AiAction::NULL_TYPE, 0, 0};
	openSet.insert(startNode);
	cameFrom.insert({ startNode.action, startNode });

	while (!openSet.empty())
	{
		Node currentNode = *(openSet.begin());
		if (currentNode.worldState.contains(goal))
		{
			return constructPlan(cameFrom, currentNode);
		}
		openSet.erase(openSet.begin());
		closedSet.insert(currentNode.action);

		for (auto& actionType : actions)
		{
			if (closedSet.contains(actionType)) continue;
			const auto& action = goap::ActionRegistry::get(actionType);
			if (currentNode.worldState.contains(action.preconditions()))
			{
				const int distanceSoFar = currentNode.distance + 1;
				const int distanceFromTarget = action.effects().distance(goal);
				const int cost = distanceSoFar + distanceFromTarget + action.cost();
				// TODO: need to merge currentNode world state into action effects
				WorldState newState = currentNode.worldState;
				newState.merge(action.effects());
				Node neighbor = Node({ newState, actionType, distanceSoFar, cost });

				auto inOpenSet = std::find_if(openSet.begin(), openSet.end(),
					[&neighbor](const Node& node) -> bool
					{
						return (neighbor.action == node.action
						&& neighbor.worldState.isSameAs(node.worldState));
					});
				if (inOpenSet == openSet.end())
				{
					openSet.insert(neighbor);
					cameFrom.insert({ actionType, currentNode });
				}
				else
				{
					if (inOpenSet->distance > distanceSoFar)
					{
						openSet.erase(inOpenSet);
						openSet.insert(neighbor);
						cameFrom.insert({ actionType, currentNode });
					}
				}
			}
		}
	}

	return std::nullopt;
}
