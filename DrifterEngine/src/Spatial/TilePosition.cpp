#include "pch.h"
#include "TilePosition.h"


drft::TilePosition::TilePosition(sf::Vector3i position)
{
    x = position.x;
    y = position.y;
    z = position.z;
}

std::string drft::TilePosition::toString(const TilePosition& tilePosition)
{
    return "(" + std::to_string(tilePosition.x) + ", " + std::to_string(tilePosition.y) + ", " + std::to_string(tilePosition.z) + ")";
}

drft::TilePosition drft::operator-(const TilePosition& left)
{
    return TilePosition{-left.x, -left.y, -left.z};
}

drft::TilePosition drft::operator+=(TilePosition& left, const TilePosition& right)
{
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;
    return left;
}

drft::TilePosition drft::operator-=(TilePosition& left, const TilePosition& right)
{
    left.x -= right.x;
    left.y -= right.y;
    left.z -= right.z;
    return left;
}

drft::TilePosition drft::operator+(const TilePosition& left, const TilePosition& right)
{
    return TilePosition(left.x + right.x, left.y + right.y, left.z + right.z);
}

drft::TilePosition drft::operator-(const TilePosition& left, const TilePosition& right)
{
    return TilePosition(left.x - right.x, left.y - right.y, left.z - right.z);
}
