#include "pch.h"
#include "RandomLayer.h"
#include <Random/Random.h>

using namespace drft;

double RandomLayer::getValueAt(sf::Vector3i tilePosition)
{
	drft::rng::Random random{ getGlobalSeed() + std::hash<sf::Vector3i>()(tilePosition) };
	return random.realInRange(0.0, 1.0);
}

void drft::RandomLayer::createFromJson(const rapidjson::Value& json)
{
}
