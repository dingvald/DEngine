#include "pch.h"
#include "TGUIHelpers.h"

tgui::UIntRect drft::toUIntRect(sf::IntRect rect)
{
    return {
        static_cast<unsigned int>(rect.left),
        static_cast<unsigned int>(rect.top),
        static_cast<unsigned int>(rect.width),
        static_cast<unsigned int>(rect.height),
    };
}
