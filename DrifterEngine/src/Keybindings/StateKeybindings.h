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
	struct BoundKeys
	{
		ModifiedInput primary = {};
		ModifiedInput secondary = {};
	};
	struct ActionKeyPair
	{
		std::string actionName;
		BoundKeys boundKeys;
	};

public:
	void createFromJson(const rapidjson::Value& json) override;
	void saveToJson(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;

	void bindKeyToAction(ModifiedInput key, const std::string& actionName, BindingPosition position);
	void unbindKeyFromAction(ModifiedInput key, const std::string& actionName);
	bool isKeyBound(ModifiedInput key) const;

	std::optional<std::string> getActionForKey(ModifiedInput key) const;
	std::vector<ActionKeyPair>& getActionKeyPairs();

private:
	std::unordered_map<std::string, BoundKeys> _actionToKeys;
	std::vector<ActionKeyPair> _actionKeyPairs;
	std::unordered_map<ModifiedInput, std::string> _keyToAction;
};

