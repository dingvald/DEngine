#pragma once
#include "Utility/Math.h"
#include "ProcGen/SpawningAlgorithms/GenerationParameters.h"

struct BiomeIcon
{
	unsigned int sprite = 4;
	sf::Color color = sf::Color::Magenta;
};

class Biome
{
private:
	struct StructureProbabilityPair
	{
		std::string structure;
		// Chance from 0.0 - 1.0
		float probability = 0.5f;
	};
	struct SpawningAlgorithm
	{
		std::string name;
		drft::gen::GenerationParameters parameters;
	};
	// { entity name, map of {algorithm, params} }
	using EntityAlgorithms = std::unordered_map<std::string, SpawningAlgorithm>;
	// { category name, list of {entity, algorithm} }
	using EntityCategories = std::unordered_map<std::string, EntityAlgorithms>;

public:
	Biome(std::string name);
	void createFromJSON(const rapidjson::Value& json);
	bool containsClimateRange(const std::string& name) const;
	drft::math::Range<float> getClimateRange(const std::string& name) const;
	BiomeIcon getIcon() const;
	const EntityCategories& getEntitySpawningAlgorithms() const;
	const std::vector<StructureProbabilityPair>& getStructureProbabilities() const;

private:
	std::string _name;
	BiomeIcon _icon;
	std::unordered_map<std::string, drft::math::Range<float>> _ranges;
	std::vector<StructureProbabilityPair> _structures;
	EntityCategories _entityCategories;
};

