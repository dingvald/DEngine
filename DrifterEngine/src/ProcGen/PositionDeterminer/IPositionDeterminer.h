#pragma once
#include <vector>
#include <SFML/System/Vector3.hpp>
#include <JSON/ICreateFromJson.h>

struct DeterminerContext
{
	unsigned int seed;
};

class IPositionDeterminer : public ICreateFromJson
{
public:
	virtual void createFromJson(const rapidjson::Value& json) = 0;
	virtual std::vector<sf::Vector3i> getPositions(const DeterminerContext& context) const = 0;
};