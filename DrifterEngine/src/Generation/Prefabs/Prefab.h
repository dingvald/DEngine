#pragma once
#include <Generation/SlotPositionList.h>
#include <JSON/ICreateFromJson.h>
#include <Spatial/AABB.h>
#include <Generation/TaggedPositions.h>

struct GenerationContext;

// Represents the data generated from Tiled exports
class Prefab : ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	SlotPositionList generate(sf::Vector3i position, const GenerationContext& context) const;

	const TaggedPositions& getTaggedPositions() const;
	const sf::IntRect& getArea() const;

private:
	sf::IntRect _area;
	SlotPositionList _entitySlots;
	TaggedPositions _taggedPositions;
};