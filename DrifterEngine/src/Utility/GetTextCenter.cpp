#include "pch.h"
#include "GetTextCenter.h"

sf::Vector2f drft::util::getTextCenter(const sf::Text& text)
{
    sf::FloatRect textRect = text.getLocalBounds();
    return sf::Vector2f(textRect.left + textRect.width/2.f,
                        textRect.top + textRect.height/2.f);
}
