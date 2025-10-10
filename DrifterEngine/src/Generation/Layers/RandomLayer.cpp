#include "pch.h"
#include "RandomLayer.h"
#include <Random/Random.h>
#include <Utility/StandardLogger.h>
#include <Utility/stdHashing.h>

using namespace drft;

double RandomLayer::getValueAt(sf::Vector3i tilePosition)
{
	size_t finalSeed = getGlobalSeed();
	hash_combine(finalSeed, std::hash<sf::Vector3i>()(tilePosition));
	hash_combine(finalSeed, _seed);
	drft::rng::Random random{ finalSeed };
	return random.realInRange(0.0, 1.0);
}

void drft::RandomLayer::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("seed"))
	{
		if (json["seed"].IsUint())
		{
			_seed = json["seed"].GetUint();
		}
		else
		{
			LOG_ERROR("Random Layer seed value {} is invalid. Requires unsigned int", json["seed"].GetString());
			_seed = 0u;
		}
	}
}
