#pragma once
#include <JSON/ICreateFromJson.h>
#include <Utility/Math.h>

struct JsonValueComparison : public ICreateFromJson
{
	enum class CompareType
	{
		Less,
		Greater,
		LessOrEqual,
		GreaterOrEqual,
		Inside,
		Outside,
	};
	void createFromJson(const rapidjson::Value& json) override;
	bool satisfiesValue(float val) const;
	float distanceFromValue(float val) const;

private:
	CompareType _compareType;
	drft::math::Range<float> _range;
};