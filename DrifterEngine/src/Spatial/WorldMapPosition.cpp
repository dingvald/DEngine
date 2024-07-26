#include "pch.h"
#include "WorldMapPosition.h"
#include <Spatial/Conversions.h>

const sf::Vector2i WorldMapTileDimensions = {64, 64};

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

sf::Vector2i drft::WorldMapPosition::toTileSpace() const
{
    return { x * WorldMapTileDimensions.x, y * WorldMapTileDimensions.y };
}

sf::Vector2i drft::WorldMapPosition::toChunkSpace() const
{
    return spatial::toChunkCoordinate(toTileSpace());
}

sf::Vector2f drft::WorldMapPosition::toFloatSpace() const
{
    return spatial::toFloatSpace(toTileSpace());
}

void drft::WorldMapPosition::fromTileSpace(sf::Vector2i tilePosition)
{
    x = tilePosition.x / WorldMapTileDimensions.x;
    y = tilePosition.y / WorldMapTileDimensions.y;
}

void drft::WorldMapPosition::fromChunkSpace(sf::Vector2i chunkPosition)
{
    fromTileSpace(spatial::toTileSpace(chunkPosition));
}

void drft::WorldMapPosition::fromFloatSpace(sf::Vector2f floatPosition)
{
    fromTileSpace(spatial::toTileSpace(floatPosition));
}
