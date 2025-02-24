#pragma once
#include <JSON/ICreateFromJson.h>

enum class KeyModifier
{
	None,
	Shift,
	Ctrl
};

enum class BindingPosition
{
	Primary,
	Secondary
};

class StateKeybindings : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void saveToJson(rapidjson::Value& json) const;

	void bindKeyToAction(sf::Keyboard::Key key, KeyModifier modifier, std::string_view actionName, BindingPosition position);
	bool isKeyBound(sf::Keyboard::Key key, KeyModifier modifier) const;

	std::optional<std::string_view> getActionForKey(sf::Keyboard::Key key, KeyModifier modifier);

private:
	struct Key
	{
		KeyModifier modifier = KeyModifier::None;
		sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;

		bool operator ==(const Key& other) const
		{
			return other.modifier == this->modifier
				&& other.key == this->key;
		}
	};
	static inline const Key InvalidKeyBind = Key{};

	struct HashKey 
	{
		std::size_t operator()(const Key& key) const 
		{
			size_t pos = static_cast<int>(key.modifier) * static_cast<int>(sf::Keyboard::KeyCount);
			return pos + static_cast<size_t>(key.key);
		} 
	};

	struct BoundKeys
	{
		Key primary = {};
		Key secondary = {};
	};

	StateKeybindings::Key parseStringToKey(const std::string& input);

private:
	std::unordered_map<std::string_view, BoundKeys> _actionToKeys;
	std::unordered_map<Key, std::string_view, HashKey> _keyToAction;
};

