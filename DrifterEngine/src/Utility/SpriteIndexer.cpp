#include "pch.h"
#include "SpriteIndexer.h"

using namespace drft::util;

sf::IntRect SpriteIndexer::get(Sprite sprite, const sf::Texture& texture)
{
    sf::Vector2u spriteDimensions = { 16, 16 };

    int width_px = texture.getSize().x;
    int height_px = texture.getSize().y;
    int width_sp = width_px / spriteDimensions.x;
    int height_sp = height_px / spriteDimensions.y;

    unsigned int row = 0;
    unsigned int col = 0;

    row = static_cast<unsigned int>(sprite) / width_sp;
    col = static_cast<unsigned int>(sprite) % width_sp;

    return sf::IntRect(col*spriteDimensions.x, row*spriteDimensions.y,
        spriteDimensions.x, spriteDimensions.y);
}
