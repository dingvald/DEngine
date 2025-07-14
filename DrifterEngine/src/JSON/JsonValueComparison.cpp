#include <pch.h>
#include <JSON/JsonValueComparison.h>
#include <Utility/StandardLogger.h>

namespace evaluation_functions
{
    static bool isEqual(float val, drft::math::Range<float> range)
    {
        if (range.getMin() != range.getMin()) return false;
        return drft::math::isNear(val, range.getMin());
    }

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

    static bool isLessOrEqual(float val, drft::math::Range<float> range)
    {
        return isLess(val, range) || drft::math::isNear(val, range.getMin());
    }

    static bool isGreaterOrEqual(float val, drft::math::Range<float> range)
    {
        return isGreater(val, range) || drft::math::isNear(val, range.getMax());
    }

    static const std::unordered_map<JsonValueComparison::CompareType, bool(*)(float, drft::math::Range<float>)> EvalFunctionMap =
    {
        {JsonValueComparison::CompareType::Equal,           isEqual},  
        {JsonValueComparison::CompareType::Greater,         isGreater},
        {JsonValueComparison::CompareType::Less,            isLess},
        {JsonValueComparison::CompareType::GreaterOrEqual,  isGreaterOrEqual},
        {JsonValueComparison::CompareType::LessOrEqual,     isLessOrEqual},
        {JsonValueComparison::CompareType::Inside,          isInside},
        {JsonValueComparison::CompareType::Outside,         isOutside},
    };
}

namespace
{
    static const std::unordered_map<std::string, JsonValueComparison::CompareType> StringToCompareType =
    {
        {"equal", JsonValueComparison::CompareType::Equal},
        {"greater", JsonValueComparison::CompareType::Greater},
        {"greater_or_equal", JsonValueComparison::CompareType::GreaterOrEqual},
        {"less", JsonValueComparison::CompareType::Less},
        {"less_or_equal", JsonValueComparison::CompareType::LessOrEqual},
        {"inside", JsonValueComparison::CompareType::Inside},
        {"outside", JsonValueComparison::CompareType::Outside},
    };
}

namespace
{
    std::optional<JsonValueComparison::CompareType> parseCompareType(const std::string& str)
    {
        if (!StringToCompareType.contains(str)) return std::nullopt;
        return StringToCompareType.at(str);
    }
}

void JsonValueComparison::createFromJson(const rapidjson::Value& json)
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

        auto comparisonType = parseCompareType(comparison);
        if (!comparisonType.has_value())
        {
            LOG_ERROR("Unknown compare type {} in comparison expression", comparison);
            return;
        }

        _compareType = comparisonType.value();

        if (_compareType == CompareType::Equal
            || _compareType == CompareType::Less 
            || _compareType == CompareType::Greater
            || _compareType == CompareType::LessOrEqual
            || _compareType == CompareType::GreaterOrEqual)
        {
            if (thresholdExpression.Size() == 2)
            {
                if (thresholdExpression[1].IsNumber())
                {
                    float rangeJson = thresholdExpression[1].GetFloat();
                    _range.setMin(rangeJson);
                    _range.setMax(rangeJson);
                }
                else if (thresholdExpression[1].IsBool())
                {
                    bool val = thresholdExpression[1].GetBool();
                    _range.setMin(val ? 1.0 : 0.0);
                    _range.setMax(val ? 1.0 : 0.0);
                }
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

bool JsonValueComparison::satisfiesValue(float val) const
{
    return evaluation_functions::EvalFunctionMap.at(_compareType)(val, _range);
}

float JsonValueComparison::distanceFromValue(float val) const
{
    if (satisfiesValue(val)) return 0.f;

    switch (_compareType)
    {
    case CompareType::Equal:
    case CompareType::Less:
    case CompareType::LessOrEqual:
        return std::abs(val - _range.getMax());
        break;
    case CompareType::Greater:
    case CompareType::GreaterOrEqual:
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
