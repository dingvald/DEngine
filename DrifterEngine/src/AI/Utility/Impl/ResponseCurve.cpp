#include "pch.h"
#include "ResponseCurve.h"
#include <Utility/StandardLogger.h>

static const float EULER_NUMBER = static_cast<float>(std::numbers::e);

static const std::unordered_map<std::string_view, ResponseCurve::Type> CurveNamesMap =
{
    {"linear",      ResponseCurve::Type::Linear},
    {"quadratic",   ResponseCurve::Type::Quadratic},
    {"logistic",    ResponseCurve::Type::Logistic},
    {"logit",       ResponseCurve::Type::Logit},
};

ResponseCurve::ResponseCurve(Type type, float m, float k, float b, float c)
    : _type(type)
    , _m(m)
    , _k(k)
    , _b(b)
    , _c(c)
{}

void ResponseCurve::createFromJson(const rapidjson::Value & json)
{
    if (json.IsArray())
    {
        const auto array = json.GetArray();
        if (array.Size() != 5)
        {
            LOG_ERROR("Response curves require 5 parameters in the array");
            return;
        }
        auto typeName = array[0].GetString();
        if (!CurveNamesMap.contains(typeName))
        {
            LOG_WARNING("Response curve contains unknown curve type '{}'. Defaulting to linear", typeName);
            _type = Type::Linear;
        }
        else
        {
            _type = CurveNamesMap.at(array[0].GetString());
        }

        _m = array[1].GetFloat();
        _k = array[2].GetFloat();
        _b = array[3].GetFloat();
        _c = array[4].GetFloat();
    }
    else
    {
        LOG_ERROR("Could not parse response curve - values must be in an array");
    }
}

float ResponseCurve::calculate(float input) const
{
    const float clampedInput    = std::clamp(input, 0.0f, 1.0f);
    const float output          = this->doCalculate(clampedInput);
    const float clampedOutput   = std::clamp(output, 0.0f, 1.0f);
    return clampedOutput;
}

float ResponseCurve::doCalculate(float input) const
{
    switch (_type)
    {
    case ResponseCurve::Type::Linear:
        return _m * std::powf(input - _c, _k) + _b;
    case ResponseCurve::Type::Quadratic:
        return _m * std::powf(input - _c, _k) + _b;
    case ResponseCurve::Type::Logistic:
        return (_k * (1.f / (1.f + ( 1000.f * EULER_NUMBER * std::powf(_m, -1.f * input + _c) )))) + _b;
    case ResponseCurve::Type::Logit:
        return (std::logf((input + _c) / (1.f - input + _c))) + _b;
    default:
        break;
    }
    return 0.f;
}
