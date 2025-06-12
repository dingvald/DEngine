#pragma once
#include <ProcGen/SlotPositionList.h>
#include <JSON/ICreateFromJson.h>
#include <Spatial/AABB.h>
#include <ProcGen/TaggedPositions.h>

struct GenerationContext;

// Represents the data generated from Tiled exports
class Prefab : ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value & json) override;

	SlotPositionList generate(sf::Vector3i position, const GenerationContext& context) const;

	const drft::spatial::AABB<int>& getVolume() const;

private:
	drft::spatial::AABB<int> _volume;
	std::vector<SlotPositionList> _layers;
};