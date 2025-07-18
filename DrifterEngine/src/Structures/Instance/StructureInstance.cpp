#include "pch.h"
#include "StructureInstance.h"

StructureInstance::StructureInstance(const StructureTemplate& structureTemplate, sf::Vector3i origin)
	: _template(&structureTemplate)
	, _origin(origin)
{

}

const drft::spatial::AABBi& StructureInstance::getVolume() const
{
	return _template->getVolume().offset(_origin);
}

const StructureInstanceLayer* StructureInstance::getOrGenerateLayer(int z)
{
	return nullptr;
}
