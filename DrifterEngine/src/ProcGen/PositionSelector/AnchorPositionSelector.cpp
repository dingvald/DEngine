#include "pch.h"
#include "AnchorPositionSelector.h"
#include <Utility/StandardLogger.h>

static const std::unordered_map<std::string, std::function<sf::Vector2i(sf::IntRect)>> AnchorPositionFunctionMap =
{
    {"center",          [](sf::IntRect rect) { return rect.getCenter(); }},
    {"top_left",        [](sf::IntRect rect) { return rect.position; }},
    {"top_right",       [](sf::IntRect rect) { return sf::Vector2i{rect.position.x + rect.size.x, rect.position.y}; }},
    {"bottom_left",     [](sf::IntRect rect) { return sf::Vector2i{rect.position.x, rect.position.y + rect.size.y}; }},
    {"bottom_right",    [](sf::IntRect rect) { return rect.position + rect.size; }},
};

void AnchorPositionSelector::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("position"))
    {
        auto& positionVal = json["position"];
        if (positionVal.IsString())
        {
            _anchorPosition = positionVal.GetString();
        }
        else if (positionVal.IsArray())
        {
            sf::Vector2i pos;
            pos.x = positionVal.GetArray()[0].GetInt();
            pos.y = positionVal.GetArray()[1].GetInt();
            _anchorPosition = pos;
        }
    }
}

std::optional<sf::Vector2i> AnchorPositionSelector::selectPosition(const sf::IntRect& area, const SelectorContext& /*context*/) const
{
    if (auto* str = std::get_if<std::string>(&_anchorPosition))
    {
        if (!AnchorPositionFunctionMap.contains(*str))
        {
            LOG_ERROR("{} is an invalid 'position' value for the anchor_selector", *str);
            return std::nullopt;
        }
        return AnchorPositionFunctionMap.at(*str)(area);
    }
    if (auto* vec = std::get_if<sf::Vector2i>(&_anchorPosition))
    {
        if (area.contains(*vec)) {
            return *vec;
        }
        return std::nullopt;
    }

    return std::nullopt;
}
