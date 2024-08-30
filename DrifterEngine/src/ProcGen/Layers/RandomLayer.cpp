#include "pch.h"
#include "RandomLayer.h"
#include <Random/Random.h>

double RandomLayer::getValueAt(sf::Vector2i tilePosition)
{
	drft::rng::Random random{ _seed + getGlobalSeed() + std::hash<sf::Vector2i>()(tilePosition) };
	return random.realInRange(0.0, 1.0);
}

void RandomLayer::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("seed"))
	{
		_seed = json["seed"].GetInt();
	}
}
