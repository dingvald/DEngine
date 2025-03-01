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
		ModifiedKey primary = {};
		ModifiedKey secondary = {};
	};
	struct ActionKeyPair
	{
		std::string actionName;
		BoundKeys boundKeys;
	};

public:
	void createFromJson(const rapidjson::Value& json) override;
	void saveToJson(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;

	void bindKeyToAction(ModifiedKey key, const std::string& actionName, BindingPosition position);
	void unbindKeyFromAction(ModifiedKey key, const std::string& actionName);
	bool isKeyBound(ModifiedKey key) const;

	std::optional<std::string> getActionForKey(ModifiedKey key) const;
	std::vector<ActionKeyPair>& getActionKeyPairs();

private:
	std::unordered_map<std::string, BoundKeys> _actionToKeys;
	std::vector<ActionKeyPair> _actionKeyPairs;
	std::unordered_map<ModifiedKey, std::string> _keyToAction;
};

