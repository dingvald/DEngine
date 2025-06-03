#pragma once
#include "Components/RenderComponent.h"
#include <JSON/ICreateFromJson.h>
#include <ProcGen/SlotDeterminer/SlotDeterminer.h>


using BiomeIcon = RenderComponent;

class Biome : public ICreateFromJson
{
public:
	Biome(std::string name);
	void createFromJson(const rapidjson::Value& json) override;

	BiomeIcon getIcon() const;
	sf::Color getBaseTileColor() const;
	const std::string& getName() const;
	
	std::vector<entt::id_type> getClimateDependencyIds() const;
	bool satisfiesClimate(const SlotDeterminer::DependencyValues& dependencyValues) const;
	float closenessToClimate(const SlotDeterminer::DependencyValues& dependencyValues) const;

	std::vector<entt::id_type> getEntitySlotDependencyIds() const;
	std::vector<entt::id_type> determineValidEntitySlots(const SlotDeterminer::DependencyValues& dependencyValues) const;

	std::vector<entt::id_type> getFeatureDependencyIds() const;
	std::vector<entt::id_type> determineValidFeatures(const SlotDeterminer::DependencyValues& dependencyValues) const;

private:
	void setBaseTileColor(sf::Color iconColor);

private:
	struct IdDeterminerPair
	{
		entt::id_type id;
		SlotDeterminer determiner;
	};

	std::string _name;
	BiomeIcon _icon;

	sf::Color _baseTileColor = sf::Color::Black;
	SlotDeterminer _climateDeterminer;
	std::vector<IdDeterminerPair> _entitySlotDeterminers;
	std::vector<IdDeterminerPair> _featureDeterminers;
};

 