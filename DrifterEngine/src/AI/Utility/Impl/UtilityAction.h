#pragma once

#include <AI/Utility/IUtilityInputProvider.h>

#include <AI/Utility/Impl/UtilityAISharedTypes.h>
#include <AI/Utility/Impl/Consideration.h>

#include <JSON/ICreateFromJson.h>


class UtilityAction : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	UtilityTargetCategoryID getTargetType() const;
	UtilityActionID getActionID() const;

	template<typename EntityType>
	float calculateScore(EntityType actor, EntityType target, const IUtilityInputProvider<EntityType>& inputProvider) const
	{
		float result = 1.f;
		for (auto&& consideration : _considerations)
		{
			const float input = inputProvider.getInput(consideration.inputID, actor, target);
			const float output = consideration.responseCurve.calculate(input);
			result *= output;
		}
		return result;
	}

private:
	UtilityActionID _id = NullUtilityAction;
	UtilityTargetCategoryID _targetType = NullUtilityTargetCategory;
	std::vector<Consideration> _considerations;
};