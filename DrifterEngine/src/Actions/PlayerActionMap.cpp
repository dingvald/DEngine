#include "pch.h"
#include "PlayerActionMap.h"


void PlayerActionMap::addAction(const std::string& name, ActionFunction function)
{
	_nameToActionFunctionMap.emplace(name, function);
}

void PlayerActionMap::mapKeyToAction(sf::Keyboard::Key key, const std::string& action)
{
	_nameToKeyMap.emplace(action, key);
	_keyToNameMap.emplace(key, action);
}

bool PlayerActionMap::isKeyAlreadyMapped(sf::Keyboard::Key key) const
{
	return _keyToNameMap.contains(key);
}

std::optional<PlayerActionMap::ActionFunction> PlayerActionMap::getActionForKey(sf::Keyboard::Key key) const
{
	if (!_keyToNameMap.contains(key)) return std::nullopt;

	const auto& name = _keyToNameMap.at(key);
	return _nameToActionFunctionMap.at(name);
}
