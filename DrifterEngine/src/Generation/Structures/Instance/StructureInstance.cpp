#include "pch.h"
#include "StructureInstance.h"
#include <Utility/StandardLogger.h>
#include <Generation/GenerationContext.h>
#include <Spatial/Helpers.h>

StructureInstance::StructureInstance(const StructureTemplate& structureTemplate, sf::Vector3i origin)
	: _template(&structureTemplate)
	, _origin(origin)
{

}

const drft::spatial::AABBi& StructureInstance::getVolume() const
{
	return _template->getVolume().offset(_origin);
}

const StructureInstanceLayer* StructureInstance::getOrGenerateLayer(int z, const GenerationContext& context)
{
	if (_layers.contains(z))
	{
		return &_layers.at(z);
	}
	const int converted_z = toTemplateZLevel(z);
	const StructureTemplateLayer* layerTemplate = _template->getLayer(converted_z);
	if (!layerTemplate) return nullptr;

	// TODO: could be a heavy operation, may need to spread across frames
	StructureInstanceLayer generatedLayer = layerTemplate->generate(drft::spatial::toXY(_origin), context);
	auto&& [val, inserted] = _layers.emplace(z, std::move(generatedLayer));
	return &(val->second);
}

int StructureInstance::toTemplateZLevel(int z) const
{
    return z - _origin.z + _template->getAnchorPoint().z;
}
