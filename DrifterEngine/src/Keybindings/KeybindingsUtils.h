#pragma once
#include <Keybindings/ModifiedKey.h>


namespace KeybindingUtils
{
	ModifiedKey getModifiedKey(sf::Keyboard::Scancode key);
	KeyModifier convertStringToKeyModifier(const std::string& input);
}
