#include "pch.h"
#include "WorldMapPosition.h"
#include <Spatial/Conversions.h>



drft::WorldMapPosition::WorldMapPosition(int x, int y)
    : x(x)
    , y(y)
{}

drft::WorldMapPosition drft::WorldMapPosition::operator+(sf::Vector2i rhs) const
{
    return WorldMapPosition{this->x + rhs.x, this->y + rhs.y};
}

drft::WorldMapPosition drft::WorldMapPosition::operator-(sf::Vector2i rhs) const
{
    return WorldMapPosition{ this->x - rhs.x, this->y - rhs.y };
}

void drft::WorldMapPosition::operator+=(sf::Vector2i rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
}

void drft::WorldMapPosition::operator-=(sf::Vector2i rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
}

bool drft::WorldMapPosition::operator==(const WorldMapPosition& other) const
{
    return (this->x == other.x && this->y == other.y);
}
