#pragma once
#include <SFML/System/Vector2.hpp>

namespace CardinalDirection
{
    inline constexpr sf::Vector2i North      = { 0, -1 };
    inline constexpr sf::Vector2i NorthEast  = { 1, -1 };
    inline constexpr sf::Vector2i East       = { 1,  0 };
    inline constexpr sf::Vector2i SouthEast  = { 1,  1 };
    inline constexpr sf::Vector2i South      = { 0,  1 };
    inline constexpr sf::Vector2i SouthWest  = { -1, 1 };
    inline constexpr sf::Vector2i West       = { -1, 0 };
    inline constexpr sf::Vector2i NorthWest  = { -1, -1 };
}