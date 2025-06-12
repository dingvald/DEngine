#include "pch.h"
#include "IDecorator.h"
#include <Random/Random.h>
#include <Utility/StandardLogger.h>

static const PositionList EmptyList = {};

void IDecorator::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("params"))
	{
		this->createFromJsonImpl(json["params"]);
	}
	else
	{
		LOG_ERROR("Decorator is missing the 'params' field");
	}
	if (json.HasMember("conditions"))
	{
		auto conditionsObj = json["conditions"].GetObject();
		_tagExpression = { conditionsObj["tag_expression"].GetString() };
		_chance = conditionsObj["chance"].GetFloat();
	}
	else
	{
		LOG_ERROR("Decorator is missing the 'conditions' field.");
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

