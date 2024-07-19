#include "pch.h"
#include "NoiseLayerMultiplier.h"


void drft::gen::NoiseLayerMultiplier::_createFromJSON(const rapidjson::Value& json)
{
	_noiseName = json["noise_name"].GetString();
	
	auto comparisonExpression = json["threshold"].GetArray();

	const std::string comparison = comparisonExpression[0].GetString();
	if (comparison == ">")
	{
		_comparisonFunc = std::greater();
	}
	else if (comparison == "<")
	{
		_comparisonFunc = std::less();
	}

	_threshold = comparisonExpression[1].GetFloat();
}

drft::gen::ProbabilityMultiplier::HitCount drft::gen::NoiseLayerMultiplier::_test(sf::Vector2i position, GenerationContext& context) const
{
	if (!context.noiseLayers.contains(_noiseName)) return 0;

	const double val = context.noiseLayers.at(_noiseName).getValueAt(position);
	if (_comparisonFunc(val, _threshold))
	{
		return 1;
	}

	return 0;
}
