#include "pch.h"
#include "ProbabilityMultiplier.h"

void drft::gen::ProbabilityMultiplier::createFromJSON(const rapidjson::Value& json)
{
	_multiplier = json["multiplier"].GetFloat();
	_createFromJSON(json);
}

float drft::gen::ProbabilityMultiplier::apply(sf::Vector2i position, GenerationContext& context) const
{
	const HitCount hitCount = _test(position, context);
	return hitCount > 0 ? (_multiplier * hitCount) : 1.0f;
}
