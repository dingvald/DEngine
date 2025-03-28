#include "pch.h"
#include "ColorToHexString.h"

std::string drft::util::colorToHexString(const sf::Color& color)
{
    std::ostringstream oss;
    oss << "#"
        << std::hex << std::uppercase << std::setfill('0')
        << std::setw(2) << static_cast<int>(color.r)
        << std::setw(2) << static_cast<int>(color.g)
        << std::setw(2) << static_cast<int>(color.b)
        << std::setw(2) << static_cast<int>(color.a);
    return oss.str();
}
