#pragma once
#include <Keybindings/ModifiedKey.h>

namespace KeybindingUtils
{
	ModifiedInput getModifiedInput(ModifiedInput::Value input);

	ModifiedInput convertStringToModifiedInput(const std::string& str);
	std::string convertModifiedInputToString(const ModifiedInput& key);
}
