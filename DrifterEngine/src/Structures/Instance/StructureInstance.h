#pragma once
#include <Structures/Template/StructureTemplate.h>
#include <Structures/Instance/StructureInstanceLayer.h>

class StructureInstance
{
public:
	StructureInstance(const StructureTemplate& structureTemplate, sf::Vector3i origin);

private:
	const StructureTemplate* _template;
	sf::Vector3i _origin;
	std::unordered_map<int, StructureInstanceLayer> _layers;
};