#pragma once

#include <ProcGen/PositionSelector/IPositionSelector.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <optional>
#include <memory>
#include <Cereal/external/rapidjson/document.h>

class AnchorPositionSelector : public IPositionSelector {
public:
    AnchorPositionSelector() = default;
    void createFromJson(const rapidjson::Value& json) override;
    std::optional<sf::Vector2i> selectPosition(const sf::IntRect& area, const SelectorContext& context) const override;
private:
    using AnchorPosition = std::variant<std::monostate, sf::Vector2i, std::string>;
    AnchorPosition _anchorPosition;
};
