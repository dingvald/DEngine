#include "pch.h"
#include "StructureDecorator.h"
#include "StructureShapeInstance.h"
#include "Random/PercentChance.h"
#include "Random/Random.h"

void drft::StructureDecorator::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("tag"))
	{
		_tag = entt::hashed_string{ json["tag"].GetString() };
	}

	if (json.HasMember("entity"))
	{
		_entityName = json["entity"].GetString();
	}

	if (json.HasMember("probability"))
	{
		_probability = json["probability"].GetFloat();
	}

	if (json.HasMember("number"))
	{
		_number.setMin(json["number"].GetArray()[0].GetInt());
		_number.setMax(json["number"].GetArray()[1].GetInt());
	}
	else
	{
		_number.setInfinite();
	}
}

void drft::StructureDecorator::apply(StructureShapeInstance& shape) const
{
	std::vector<sf::Vector2i> randomPositions;
	const auto& positions = shape.getPositionsFor(_tag);
	randomPositions.reserve(positions.size());
	randomPositions.insert(randomPositions.end(), positions.begin(), positions.end());

	std::shuffle(randomPositions.begin(), randomPositions.end(), std::default_random_engine(0));

	static rng::Random random{ rng::GlobalSeed };
	const int numberToSet = random.intInRange(_number.getMin(), _number.getMax());

	int numberSet = 0;
	for (auto&& position : randomPositions)
	{
		if (!rng::percentChance(_probability * 100.0)) continue;
		if (!_number.isInfinite() && (numberSet++ >= numberToSet)) return;

		shape.removeEntities(position);
		shape.addEntity(_entityName, position);
	}
}
