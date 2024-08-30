#pragma once
#include "Utility/Math.h"
#include "Components/RenderComponent.h"
#include <Factory/Factory.h>
#include <JSON/ICreateFromJson.h>
#include <JSON/StringExpressions.h>


using BiomeIcon = RenderComponent;
using EntityPack = std::vector<std::pair<std::string, int>>;

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

	// Inherited via ICreateFromJson
	void createFromJson(const rapidjson::Value& json) override;
};

class Biome
{
public:
	Biome(std::string name);
	void createFromJSON(const rapidjson::Value& json);
	
	bool satisfiesClimate(const std::unordered_map<std::string, float>& values) const;
	const std::unordered_map<std::string, drft::math::Range<float>>& getClimateRanges() const;
	const std::unordered_map<entt::id_type, SlotDeterminer>& getSlotDeterminers() const;
	const EntityPack& getEntityPack(entt::id_type slotID) const;

	BiomeIcon getIcon() const;
	const std::string& getName() const;

private:
	std::string _name;
	BiomeIcon _icon;

	std::unordered_map<std::string, drft::math::Range<float>> _ranges;
	std::unordered_map<entt::id_type, SlotDeterminer> _entitySlotDeterminers;
	std::unordered_map<entt::id_type, EntityPack> _entityPacks;
};

 