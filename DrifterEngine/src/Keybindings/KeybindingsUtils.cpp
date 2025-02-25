#include "pch.h"
#include "KeybindingsUtils.h"

KeyModifier getKeyModifier()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
		|| sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
	{
		return KeyModifier::Ctrl;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
		|| sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
	{
		return KeyModifier::Shift;
	}
	return KeyModifier::None;
}
