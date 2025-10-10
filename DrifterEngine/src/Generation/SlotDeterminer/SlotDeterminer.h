#pragma once
#include <JSON/ICreateFromJson.h>
#include <JSON/StringExpressions.h>
#include <JSON/JsonValueComparison.h>

struct SlotDeterminer : public ICreateFromJson
{
	using DependencyValues = std::unordered_map<entt::id_type, double>;

	void createFromJson(const rapidjson::Value& json) override;
	bool isValidSlot(const DependencyValues& values) const;
	float distanceFromValid(const DependencyValues& values) const;
	std::vector<entt::id_type> getDependencyIds() const;

private:
	std::unordered_map<entt::id_type, JsonValueComparison> _comparisons;
	drft::util::BooleanStringExpression _expression;
};