#include "pch.h"
#include "StructureTemplate.h"
#include <ProcGen/GenerationFinalizationContext.h>
#include <Utility/StandardLogger.h>

static const std::unordered_map<std::string, std::function<sf::Vector2i(sf::IntRect)>> AnchorPositionFunctionMap =
{
    {"center", [](sf::IntRect rect) { return rect.getCenter(); }},
    {"top_left", [](sf::IntRect rect) { return rect.position; }}
};

void StructureTemplate::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("layers"))
    {
        for (auto&& layerVal : json["layers"].GetArray())
        {
            int depth = 0;
            if (layerVal.HasMember("depth"))
            {
                depth = layerVal["depth"].GetInt();
            }
            StructureTemplateLayer newLayer;
            newLayer.createFromJson(layerVal);
            _layers.emplace(depth, std::move(newLayer));
        }
    }
    if (json.HasMember("anchor_point"))
    {
        AnchorPointDescription anchorPointDescription;
        anchorPointDescription.positionSelectionFunction = AnchorPositionFunctionMap.at("center"); // Default to center
        anchorPointDescription.depth = 0; // Default to 0
        if (json["anchor_point"].HasMember("position"))
        {
            const std::string position = json["anchor_point"]["position"].GetString();
            if (!AnchorPositionFunctionMap.contains(position))
            {
                LOG_ERROR("Structure anchor_point does not support position type {}, defaulting to 'center'", position);
            }
            else
            {
                anchorPointDescription.positionSelectionFunction = AnchorPositionFunctionMap.at(position);
            }
        }
        if (json["anchor_point"].HasMember("depth"))
        {
            const int depth = json["anchor_point"]["depth"].GetInt();
            if (!_layers.contains(depth))
            {
                LOG_ERROR("Structure does not contain layer at depth {} for anchor_point, defaulting to 0", depth);
            }
            else
            {
                anchorPointDescription.depth = depth;
            }
        }
        _anchorPoint = anchorPointDescription;
    }
}

void StructureTemplate::finalize(const GenerationFinalizationContext& context)
{
    for (auto&& [depth, layer] : _layers)
    {
        layer.finalize(context);
    }
    for (auto&& [depth, layer] : _layers)
    {
        sf::IntRect area = layer.getArea();
        sf::Vector3i minPoint = { area.position.x, area.position.y, depth };
        sf::Vector3i maxPoint = { area.position.x + area.size.x, area.position.y + area.size.y, depth };
        _volume = _volume.expandToFit(minPoint);
        _volume = _volume.expandToFit(maxPoint);
    }
}

const drft::spatial::AABBi& StructureTemplate::getVolume() const
{
    return _volume;
}

sf::Vector3i StructureTemplate::getAnchorPoint() const
{
    // TODO: Maybe change to use the flattened volume
    auto& layer = _layers.at(_anchorPoint.depth);
    sf::Vector2i position = _anchorPoint.positionSelectionFunction(layer.getArea());
    return sf::Vector3i{ position.x, position.y, _anchorPoint.depth };
}

const StructureTemplateLayer* StructureTemplate::getLayer(int z) const
{
    if (!_layers.contains(z))
    {
        return nullptr;
    }
    return &_layers.at(z);
}
