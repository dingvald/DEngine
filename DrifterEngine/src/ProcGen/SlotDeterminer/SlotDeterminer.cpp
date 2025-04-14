#include "pch.h"
#include "SlotDeterminer.h"

#include <Utility/StandardLogger.h>

namespace evaluation_functions
{
	static bool isGreater(float val, drft::math::Range<float> range)
	{
		return val > range.getMax();
	}

	static bool isLess(float val, drft::math::Range<float> range)
	{
		return val < range.getMin();
	}

	static bool isInside(float val, drft::math::Range<float> range)
	{
		return range.isValueWithin(val);
	}

	static bool isOutside(float val, drft::math::Range<float> range)
	{
		return !range.isValueWithin(val);
	}

	static const std::unordered_map<SlotDeterminer::Dependency::CompareType, bool(*)(float, drft::math::Range<float>)> EvalFunctionMap =
	{
		{SlotDeterminer::Dependency::CompareType::Greater, isGreater},
		{SlotDeterminer::Dependency::CompareType::Less, isLess},
		{SlotDeterminer::Dependency::CompareType::Inside, isInside},
		{SlotDeterminer::Dependency::CompareType::Outside, isOutside},
	};
}

namespace
{
	static const std::unordered_map<std::string, SlotDeterminer::Dependency::CompareType> StringToCompareType =
	{
		{"greater", SlotDeterminer::Dependency::CompareType::Greater},
		{"less", SlotDeterminer::Dependency::CompareType::Less},
		{"inside", SlotDeterminer::Dependency::CompareType::Inside},
		{"outside", SlotDeterminer::Dependency::CompareType::Outside},
	};
}

void SlotDeterminer::Dependency::createFromJson(const rapidjson::Value& json)
{
	if (json.IsArray())
	{
		auto thresholdExpression = json.GetArray();
		if (thresholdExpression.Size() < 2)
		{
			LOG_ERROR("Malformed comparison expression - not enough arguments");
			return;
		}

		std::string comparison = thresholdExpression[0].GetString();

		if (!StringToCompareType.contains(comparison))
		{
			LOG_ERROR("Unknown compare type {} in comparison expression", comparison);
			return;
		}

		_compareType = StringToCompareType.at(comparison);

		if (_compareType == CompareType::Less || _compareType == CompareType::Greater)
		{
			if (thresholdExpression.Size() == 2)
			{
				float rangeJson = thresholdExpression[1].GetFloat();
				_range.setMin(rangeJson);
				_range.setMax(rangeJson);
			}
			else
			{
				LOG_ERROR("Incorrect number of arguments for comparison type {} - expected 1 but got {}", comparison, thresholdExpression.Size() - 1);
			}
		}
		else
		{
			if (thresholdExpression.Size() == 3)
			{
				float range_min = thresholdExpression[1].GetFloat();
				float range_max = thresholdExpression[2].GetFloat();
				_range.setMin(range_min);
				_range.setMax(range_max);
			}
			else
			{
				LOG_ERROR("Incorrect number of arguments for comparison type {} - expected 2 but got {}", comparison, thresholdExpression.Size() - 1);
			}
		}
	}
}

bool SlotDeterminer::Dependency::satisfiesValue(float val) const
{
	return evaluation_functions::EvalFunctionMap.at(_compareType)(val, _range);
}

float SlotDeterminer::Dependency::distanceFromValue(float val) const
{
	if (satisfiesValue(val)) return 0.f;

	switch (_compareType)
	{
	case CompareType::Less:
		return std::abs(val - _range.getMax());
		break;
	case CompareType::Greater:
		return std::abs(val - _range.getMin());
		break;
	case CompareType::Inside:
		if (val < _range.getMin())
		{
			return std::abs(val - _range.getMin());
		}
		else
		{
			return std::abs(val - _range.getMax());
		}
		break;
	case CompareType::Outside:
		return std::min(std::abs(val - _range.getMin()), std::abs(val - _range.getMax()));
		break;
	default:
		break;
	}

	return _range.distance(val);
}

void SlotDeterminer::createFromJson(const rapidjson::Value& json)
{
	for (auto&& member : json.GetObject())
	{
		if (member.name == "expression")
		{
			expression = drft::util::BooleanStringExpression{ json["expression"].GetString() };
			continue;
		}

		Dependency newDependency;
		newDependency.createFromJson(member.value);
		dependencies.emplace(entt::hashed_string{ member.name.GetString() }, std::move(newDependency));
	}
}
