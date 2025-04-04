#pragma once
#include "Utility/Math.h"
#include "Components/RenderComponent.h"
#include <Factory/Factory.h>
#include <JSON/ICreateFromJson.h>
#include <JSON/StringExpressions.h>


using BiomeIcon = RenderComponent;

struct SlotDependency : public ICreateFromJson
{
	std::function<bool(float, drft::math::Range<float>)> satisfiesValue;
	drft::math::Range<float> range;
	void createFromJson(const rapidjson::Value& json) override;
};

struct SlotDeterminer : public ICreateFromJson
{
	std::unordered_map<entt::id_type, SlotDependency> dependencies;
	drft::util::BooleanStringExpression expression;
	void createFromJson(const rapidjson::Value& json) override;
};

class Biome
{
public:
	Biome(std::string name);
	void createFromJSON(const rapidjson::Value& json);
	
	bool satisfiesClimate(const std::unordered_map<entt::id_type, float>& values) const;
	float closenessToClimate(const std::unordered_map<entt::id_type, float>& values) const;
	const std::unordered_map<entt::id_type, drft::math::Range<float>>& getClimateRanges() const;
	const std::unordered_map<entt::id_type, SlotDeterminer>& getSlotDeterminers() const;

	BiomeIcon getIcon() const;
	sf::Color getBaseTileColor() const;
	const std::string& getName() const;

private:
	void setBaseTileColor(sf::Color iconColor);

private:
	std::string _name;
	BiomeIcon _icon;

	sf::Color _baseTileColor = sf::Color::Black;
	std::unordered_map<entt::id_type, drft::math::Range<float>> _ranges;
	std::unordered_map<entt::id_type, SlotDeterminer> _entitySlotDeterminers;
};

 