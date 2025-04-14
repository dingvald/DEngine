#pragma once
#include <JSON/ICreateFromJson.h>
#include <JSON/StringExpressions.h>
#include <Utility/Math.h>


struct SlotDeterminer : public ICreateFromJson
{
	struct Dependency : public ICreateFromJson
	{
		enum class CompareType
		{
			Less,
			Greater,
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

	std::unordered_map<entt::id_type, Dependency> dependencies;
	drft::util::BooleanStringExpression expression;
	void createFromJson(const rapidjson::Value& json) override;
};