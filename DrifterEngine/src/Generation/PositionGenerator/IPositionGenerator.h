#pragma once
#include <JSON/ICreateFromJson.h>
#include <unordered_set>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector3.hpp>
#include <Utility/stdHashing.h>

struct PositionGeneratorContext
{
	unsigned int seed;
	sf::IntRect area;
};

class IPositionGenerator : public ICreateFromJson
{
public:
	using PositionSet = std::unordered_set<sf::Vector3i>;
	virtual void createFromJson(const rapidjson::Value& json) = 0;
	virtual PositionSet generatePositions(const PositionGeneratorContext& context) const = 0;
};