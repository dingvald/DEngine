#include "pch.h"
#include "StructureTemplate.h"

void StructureTemplate::createFromJson(const rapidjson::Value& json)
{
}

const drft::spatial::AABBi& StructureTemplate::getVolume() const
{
    return _volume;
}

sf::Vector3i StructureTemplate::getAnchorPoint() const
{
    return _anchorPoint;
}

const StructureTemplateLayer* StructureTemplate::getLayer(int z) const
{
    if (!_layers.contains(z))
    {
        return nullptr;
    }
    return &_layers.at(z);
}
