#include "pch.h"
#include "KeybindingsUtils.h"

static const std::unordered_map<std::string, KeyModifier> StringToKeyModifier =
{
	{"Shift", KeyModifier::Shift},
	{"Ctrl", KeyModifier::Ctrl}
};

static const std::unordered_set<sf::Keyboard::Scancode> SfmlKeyModifiers =
{
	sf::Keyboard::Scancode::LShift,
	sf::Keyboard::Scancode::RShift,
	sf::Keyboard::Scancode::LControl,
	sf::Keyboard::Scancode::RControl
};

ModifiedKey KeybindingUtils::getModifiedKey(sf::Keyboard::Scancode key)
{
	if (SfmlKeyModifiers.contains(key)) return {};

	ModifiedKey result;
	result.key = key;
	result.modifier = KeyModifier::None;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
		|| sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
	{
		result.modifier = KeyModifier::Ctrl;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
		|| sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
	{
		result.modifier = KeyModifier::Shift;
	}

	return result;
}

KeyModifier KeybindingUtils::convertStringToKeyModifier(const std::string& input)
{
	if (!StringToKeyModifier.contains(input)) return KeyModifier::None;

	return StringToKeyModifier.at(input);
}
