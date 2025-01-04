#pragma once
#include <JSON/ICreateFromJson.h>

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