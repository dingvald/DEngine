#pragma once
#include <JSON/ICreateFromJson.h>
#include <Utility/stdHashing.h>
#include <Keybindings/KeybindingsUtils.h>
#include <Keybindings/ModifiedKey.h>

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

	void bindKeyToAction(ModifiedKey key, entt::hashed_string actionName, BindingPosition position);
	bool isKeyBound(ModifiedKey key) const;

	std::optional<entt::hashed_string> getActionForKey(ModifiedKey key) const;

private:
	static inline const ModifiedKey InvalidKeyBind = {};

	struct BoundKeys
	{
		ModifiedKey primary = {};
		ModifiedKey secondary = {};
	};

	ModifiedKey convertStringToKey(const std::string& input) const;
	std::string convertKeyToString(ModifiedKey key) const;

private:
	std::unordered_map<entt::hashed_string, BoundKeys> _actionToKeys;
	std::unordered_map<ModifiedKey, entt::hashed_string> _keyToAction;
};

