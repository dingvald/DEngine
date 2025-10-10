#include "pch.h"
#include "SlotDeterminer.h"
#include <Utility/ContainerHelpers.h>

void SlotDeterminer::createFromJson(const rapidjson::Value& json)
{
	for (auto&& member : json.GetObject())
	{
		if (member.name == "expression")
		{
			_expression = drft::util::BooleanStringExpression{ json["expression"].GetString() };
			continue;
		}

		JsonValueComparison newComparison;
		newComparison.createFromJson(member.value);
		_comparisons.emplace(entt::hashed_string{ member.name.GetString() }, std::move(newComparison));
	}
}

bool SlotDeterminer::isValidSlot(const DependencyValues& values) const
{
	TokenValues tokenValues;
	for (auto&& [layerID, comparison] : _comparisons)
	{
		if (!values.contains(layerID)) return false;

		tokenValues.emplace(layerID, comparison.satisfiesValue(values.at(layerID)));
	}
	return _expression.evaluate(tokenValues);
}

float SlotDeterminer::distanceFromValid(const DependencyValues& values) const
{
	float result = 0.f;
	for (auto&& [id, val] : values)
	{
		if (!_comparisons.contains(id))
		{
			result += 1.0f;
			continue;
		}
		result += _comparisons.at(id).distanceFromValue(val);
	}
	return result;
}

std::vector<entt::id_type> SlotDeterminer::getDependencyIds() const
{
	return drft::util::extractKeys(_comparisons);
}
