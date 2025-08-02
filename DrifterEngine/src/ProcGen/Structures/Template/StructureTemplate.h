#pragma once
#include <ProcGen/Structures/Template/StructureTemplateLayer.h>
#include <JSON/ICreateFromJson.h>
#include <Spatial/AABB.h>

struct GenerationFinalizationContext;

class StructureTemplate : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void finalize(const GenerationFinalizationContext& context);

	const drft::spatial::AABBi& getVolume() const;
	sf::Vector3i getAnchorPoint() const;

	const StructureTemplateLayer* getLayer(int z) const;

private:
	std::unordered_map<int, StructureTemplateLayer> _layers;
	struct AnchorPointDescription
	{
		std::function<sf::Vector2i(sf::IntRect)> positionSelectionFunction;
		int depth;
	};
	AnchorPointDescription _anchorPoint;
	drft::spatial::AABBi _volume;
};