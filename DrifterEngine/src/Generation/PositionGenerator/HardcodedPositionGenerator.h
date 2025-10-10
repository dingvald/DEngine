#pragma once
#include <Generation/PositionGenerator/IPositionGenerator.h>

class HardcodedPositionGenerator : public IPositionGenerator
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	PositionSet generatePositions(const PositionGeneratorContext& context) const override;

private:
	sf::Vector3i _location;
};