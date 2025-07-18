#pragma once
#include <Structures/Template/StructureTemplateLayer.h>
#include <JSON/ICreateFromJson.h>
#include <Spatial/AABB.h>

class StructureTemplate : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;

	const drft::spatial::AABBi& getVolume() const;
	sf::Vector3i getAnchorPoint() const;

	const StructureTemplateLayer* getLayer(int z) const;

private:
	std::unordered_map<int, StructureTemplateLayer> _layers;
	std::unordered_map<sf::Vector3i, entt::id_type> _layerLinks;

	sf::Vector3i _anchorPoint = { 0,0,0 };
	drft::spatial::AABBi _volume;
};