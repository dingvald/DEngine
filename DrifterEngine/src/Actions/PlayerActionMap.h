#pragma once

class PlayerActionMap
{
public:
	using ActionFunction = std::function<void(entt::handle)>;

	void addAction(const std::string& name, ActionFunction function);
	void mapKeyToAction(sf::Keyboard::Key key, const std::string& action);
	bool isKeyAlreadyMapped(sf::Keyboard::Key key) const;

	std::optional<ActionFunction> getActionForKey(sf::Keyboard::Key key) const;

private:
	std::unordered_map<std::string, sf::Keyboard::Key> _nameToKeyMap;
	std::unordered_map<sf::Keyboard::Key, std::string> _keyToNameMap;
	std::unordered_map<std::string, ActionFunction> _nameToActionFunctionMap;
};

