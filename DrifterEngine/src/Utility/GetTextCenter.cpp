#include "pch.h"
#include "GetTextCenter.h"

sf::Vector2f drft::util::getTextCenter(const sf::Text& text)
{
    sf::FloatRect textRect = text.getLocalBounds();
    return textRect.getCenter();
}
