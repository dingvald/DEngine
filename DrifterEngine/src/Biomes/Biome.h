#pragma once
#include "Utility/Math.h"
#include "Components/RenderComponent.h"
#include <Factory/Factory.h>


using BiomeIcon = RenderComponent;

class Biome
{
public:
	Biome(std::string name);
	void createFromJSON(const rapidjson::Value& json);
	
	float distanceFromClimate(const std::unordered_map<std::string, float>& values) const;
	const std::unordered_map<std::string, drft::math::Range<float>>& getClimateRanges() const;

	BiomeIcon getIcon() const;
	const std::string& getName() const;

private:
	std::string _name;
	BiomeIcon _icon;
	std::unordered_map<std::string, drft::math::Range<float>> _ranges;
};

