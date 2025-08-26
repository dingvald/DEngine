#pragma once

#include <JSON/ICreateFromJson.h>
#include <AI/Utility/IUtilityInputProvider.h>
#include <AI/Utility/IUtilityBlackboard.h>
#include <AI/Utility/UtilityActionTargetPair.h>

#include <AI/Utility/Impl/UtilityAction.h>

class UtilityArchetype : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	template<typename EntityType>
	std::multimap<float, UtilityActionTargetPair<EntityType>, std::greater<float>> scoreActions(EntityType entity, const IUtilityBlackboard<EntityType>& blackboard, const IUtilityInputProvider<EntityType>& inputProvider) const
	{
		std::multimap<float, UtilityActionTargetPair<EntityType>, std::greater<float>> result;
		for (auto&& action : _actions)
		{
			auto& targetData = blackboard.getList(AiTargetTypes::toIdHash(action.getTargetType()));
			if (targetData.empty())
			{
				// No targets for this target type, so score as a ZERO
				result.emplace(0.f, UtilityActionTargetPair<EntityType>{action.getActionID(), entity});
			}
			else
			{
				for (auto&& data : targetData)
				{
					if (auto target = std::get_if<EntityType>(&data))
					{
						float score = action.calculateScore(entity, *target, inputProvider);
						result.emplace(score, UtilityActionTargetPair<EntityType>{action.getActionID(), * target});
					}
				}
			}
		}
		return result;
	}

private:
	std::vector<UtilityAction> _actions;
};