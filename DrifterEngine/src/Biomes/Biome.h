#pragma once
#include "Utility/Math.h"
#include "ProcGen/ProbabilityMultipliers/ProbabilityMultiplier.h"
#include "Components/RenderComponent.h"
#include <Factory/Factory.h>


using BiomeIcon = RenderComponent;
using MuliplierFactory = Factory<drft::gen::ProbabilityMultiplier>;

class Biome
{
private:
	using EntityWeight = std::pair<std::string, int>;

	struct StructureProbabilityPair
	{
		std::string structure;
		// Chance from 0.0 - 1.0
		float probability = 0.5f;
	};

	struct EntitySlot
	{
		using MultiplierPtr = std::unique_ptr<drft::gen::ProbabilityMultiplier>;
		std::string name;
		float probability = 0.0f;
		std::vector<MultiplierPtr> multipliers;
	};

public:
	Biome(std::string name);
	void createFromJSON(const rapidjson::Value& json);
	bool containsClimateRange(const std::string& name) const;
	drft::math::Range<float> getClimateRange(const std::string& name) const;

	// Checks if the biome satisfies the climate value
	bool satisfiesClimate(const std::string& climateName, float value) const;
	float getDeviationFromClimate(const std::string& climateName, float value) const;

	BiomeIcon getIcon() const;
	const std::string& getName() const;
	const std::unordered_map<entt::id_type, EntitySlot>& getEntitySlots() const;
	const std::unordered_map<entt::id_type, std::vector<EntityWeight>>& getEntityPacks() const;
	const std::vector<StructureProbabilityPair>& getStructureProbabilities() const;

private:
	std::string _name;
	BiomeIcon _icon;
	std::unordered_map<std::string, drft::math::Range<float>> _ranges;
	std::unordered_map<entt::id_type, EntitySlot> _entitySlots;
	std::unordered_map<entt::id_type, std::vector<EntityWeight>> _entityPacks;
	std::vector<StructureProbabilityPair> _structures;
};

