#include "pch.h"
#include "IDecorator.h"
#include <Random/Random.h>

static const PositionList EmptyList = {};

void IDecorator::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("params"))
	{
		this->createFromJsonImpl(json["params"]);
	}
	if (json.HasMember("conditions"))
	{
		auto conditionsObj = json["conditions"].GetObject();
		_tag = entt::hashed_string{ conditionsObj["tag"].GetString() };
		_chance = conditionsObj["chance"].GetFloat();
	}
}

const PositionList& IDecorator::getMyPositions(const TaggedPositions& taggedPositions) const
{
	if (!taggedPositions.contains(_tag)) return EmptyList;
	return taggedPositions.at(_tag);
}

bool IDecorator::meetsCondition(drft::rng::Random& random) const
{
	return (random.realInRange(0.0f, 1.0f) < _chance);
}
