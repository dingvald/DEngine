#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <Utility/stdHashing.h>

enum class KeyModifier
{
	None,
	Shift,
	Ctrl
};

struct ModifiedInput
{
	using Value = std::variant<std::monostate, sf::Keyboard::Scancode, sf::Mouse::Button>;
	Value value = std::monostate{};
	KeyModifier modifier = KeyModifier::None;

	bool operator ==(const ModifiedInput& other) const
	{
		return other.modifier == this->modifier
			&& other.value == this->value;
	}
	operator bool() const
	{
		return static_cast<bool>(value.index());
	}
};

static const ModifiedInput InvalidKey = ModifiedInput{};

template<>
struct std::hash<ModifiedInput>
{
	std::size_t operator()(const ModifiedInput& input) const noexcept
	{
		size_t variantHash = std::hash<ModifiedInput::Value>{}(input.value);
		hash_combine(variantHash, input.modifier);
		return variantHash;
	}
};