#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <memory> // For std::hash definition

enum class KeyModifier
{
	None,
	Shift,
	Ctrl
};

struct ModifiedKey
{
	sf::Keyboard::Scancode key = sf::Keyboard::Scancode::Unknown;
	KeyModifier modifier = KeyModifier::None;

	bool operator ==(const ModifiedKey& other) const
	{
		return other.modifier == this->modifier
			&& other.key == this->key;
	}
};

template<>
struct std::hash<ModifiedKey>
{
	std::size_t operator()(const ModifiedKey& key) const noexcept
	{
		size_t pos = static_cast<int>(key.modifier) * static_cast<int>(sf::Keyboard::Scancode::ScancodeCount);
		return pos + static_cast<size_t>(key.key);
	}
};