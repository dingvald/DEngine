#pragma once
#include <Keybindings/ModifiedKey.h>

namespace KeybindingUtils
{
	ModifiedKey getModifiedKey(sf::Keyboard::Scancode key);

	ModifiedKey convertStringToModifiedKey(const std::string& str);
	std::string convertModifiedKeyToString(const ModifiedKey& key);
}
