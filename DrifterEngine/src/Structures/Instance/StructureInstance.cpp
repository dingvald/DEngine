#include "pch.h"
#include "StructureInstance.h"

StructureInstance::StructureInstance(const StructureTemplate& structureTemplate, sf::Vector3i origin)
	: _template(&structureTemplate)
	, _origin(origin)
{
}
