#pragma once
#include <JSON/ICreateFromJson.h>

/// <summary>
/// Represents a mathematical response curve with configurable parameters.
/// Calulates a response and clamps the output between [0.0, 1.0]
/// </summary>
class ResponseCurve : public ICreateFromJson
{
public:
	enum class Type
	{
		Linear,
		Quadratic,
		Logistic,
		Logit
	};
public:
	ResponseCurve() = default;
	ResponseCurve(Type type, float m, float k, float b, float c);

	void createFromJson(const rapidjson::Value& json) override;

	float calculate(float input) const;

private:
	float doCalculate(float input) const;

private:
	Type _type = Type::Linear;
	float _m = 1.0f;
	float _k = 1.0f;
	float _b = 0.0f;
	float _c = 0.0f;
};