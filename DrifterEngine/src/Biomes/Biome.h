#pragma once
#include "Utility/Math.h"
#include "Components/RenderComponent.h"
#include <JSON/ICreateFromJson.h>
#include <JSON/StringExpressions.h>


using BiomeIcon = RenderComponent;

struct SlotDependency : public ICreateFromJson
{
	enum class CompareType
	{
		Less,
		Greater,
		Inside,
		Outside,
	};
	void createFromJson(const rapidjson::Value& json) override;
	bool satisfiesValue(float val) const;
	float distanceFromValue(float val) const;

private:
	CompareType _compareType;
	drft::math::Range<float> _range;
};

struct SlotDeterminer : public ICreateFromJson
{
	std::unordered_map<entt::id_type, SlotDependency> dependencies;
	drft::util::BooleanStringExpression expression;
	void createFromJson(const rapidjson::Value& json) override;
};

class Biome : public ICreateFromJson
{
public:
	Biome(std::string name);
	void createFromJson(const rapidjson::Value& json) override;

	BiomeIcon getIcon() const;
	sf::Color getBaseTileColor() const;
	const std::string& getName() const;
	
	std::vector<entt::id_type> getClimateDependencyIds() const;
	bool satisfiesClimate(const std::unordered_map<entt::id_type, float>& dependencyValues) const;
	float closenessToClimate(const std::unordered_map<entt::id_type, float>& dependencyValues) const;

	std::vector<entt::id_type> getEntitySlotDependencyIds() const;
	std::vector<entt::id_type> determineValidEntitySlots(const std::unordered_map<entt::id_type, float>& dependencyValues) const;

	std::vector<entt::id_type> getFeatureDependencyIds() const;
	std::vector<entt::id_type> determineValidFeature(const std::unordered_map<entt::id_type, float>& dependencyValues) const;

private:
	void setBaseTileColor(sf::Color iconColor);

private:
	std::string _name;
	BiomeIcon _icon;

	sf::Color _baseTileColor = sf::Color::Black;
	SlotDeterminer _climateDeterminer;
	std::unordered_map<entt::id_type, SlotDeterminer> _entitySlotDeterminers;
	std::unordered_map<entt::id_type, SlotDeterminer> _featureDeterminers;
};

 