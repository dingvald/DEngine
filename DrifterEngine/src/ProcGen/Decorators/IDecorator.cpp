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
		_tagExpression = { conditionsObj["tag_expression"].GetString() };
		_chance = conditionsObj["chance"].GetFloat();
	}
}

PositionList IDecorator::getMyPositions(const TaggedPositions& taggedPositions) const
{
	return _tagExpression.mergeLists(taggedPositions);
}

bool IDecorator::meetsCondition(drft::rng::Random& random) const
{
	return (random.realInRange(0.0f, 1.0f) < _chance);
}
