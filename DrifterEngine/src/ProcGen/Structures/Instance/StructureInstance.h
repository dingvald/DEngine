#pragma once
#include <ProcGen/Structures/Template/StructureTemplate.h>
#include <ProcGen/Structures/Instance/StructureInstanceLayer.h>
#include <Spatial/AABB.h>

struct GenerationContext;

/// <summary>
/// Represents a structure that has been placed at position x, y, z
/// </summary>
class StructureInstance
{
public:
	StructureInstance(const StructureTemplate& structureTemplate, sf::Vector3i origin);

	const drft::spatial::AABBi& getVolume() const;
	const StructureInstanceLayer* getOrGenerateLayer(int z, const GenerationContext& context);

private:
	int toTemplateZLevel(int z) const;

private:
	const StructureTemplate* _template;
	sf::Vector3i _origin;
	std::unordered_map<int, StructureInstanceLayer> _layers;
};