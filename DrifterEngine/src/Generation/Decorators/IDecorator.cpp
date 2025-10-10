#include "pch.h"
#include "IDecorator.h"
#include <Random/Random.h>
#include <Utility/StandardLogger.h>
#include <Generation/GenerationContext.h>

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
	if (json.HasMember("priority"))
	{
		_priority = json["priority"].GetInt();
	}
}

void IDecorator::decorate(SlotPositionList& inOutSlotPositions, const TaggedPositions& taggedPositions, const GenerationContext& context) const
{
	SlotPositionList result = this->decorateImpl(getMyPositions(taggedPositions, context.seed), context);
	for (auto&& slot : result)
	{
		slot.priority = _priority;
	}
	inOutSlotPositions.append_range(result);
}

size_t IDecorator::generateUniqueSeed(size_t fromSeed) const
{
	hash_combine(fromSeed, _tagExpression.getHash());
	hash_combine(fromSeed, _priority);
	hash_combine(fromSeed, _chance);
	return fromSeed;
}

PositionList IDecorator::getMyPositions(const TaggedPositions& taggedPositions, size_t globalSeed) const
{
	PositionList result;
	PositionList temp = _tagExpression.mergeLists(taggedPositions);
	if (temp.empty()) return result;

	drft::rng::Random random{ generateUniqueSeed(globalSeed)};

	for (auto&& pos : temp)
	{
		if (!meetsCondition(random)) continue;
		result.push_back(pos);
	}
	return result;
}

bool IDecorator::meetsCondition(drft::rng::Random& random) const
{
	return (random.realInRange(0.0f, 1.0f) < _chance);
}

