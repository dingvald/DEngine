#pragma once

#include <JSON/ICreateFromJson.h>
#include <AI/Utility/IUtilityInputProvider.h>
#include <AI/Utility/IUtilityBlackboard.h>
#include <AI/Utility/ScoredUtilityAction.h>

#include <AI/Utility/Impl/UtilityAction.h>

class UtilityArchetype : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	template<typename EntityType>
	std::vector<ScoredUtilityAction<EntityType>> scoreActions(EntityType entity, const IUtilityBlackboard<EntityType>& blackboard, const IUtilityInputProvider<EntityType>& inputProvider) const
	{
		std::vector<ScoredUtilityAction<EntityType>> result;
		for (auto&& action : _actions)
		{
			auto& targetData = blackboard.getList(AiTargetTypes::toIdHash(action.getTargetType()));
			if (targetData.empty()) continue;

			for (auto&& data : targetData)
			{
				if (const EntityType* target = std::get_if<EntityType>(&data))
				{
					float score = action.calculateScore(entity, *target, inputProvider);
					result.emplace_back(UtilityActionTargetPair<EntityType>{action.getActionID(), * target}, score);
				}
			}
		}

		return result;
	}

private:
	std::vector<UtilityAction> _actions;
};