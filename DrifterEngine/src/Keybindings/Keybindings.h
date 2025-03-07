#pragma once
#include <Keybindings/StateKeybindings.h>

class Keybindings : public ICreateFromJson
{
public:
	void createDefaultsFromJson(const rapidjson::Value& json);
	void createFromJson(const rapidjson::Value& json) override;

	void saveToJson(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const;

	bool containsState(const std::string& state) const;

	const StateKeybindings& operator [](const std::string& state) const;

	const StateKeybindings& forState(const std::string& state) const;
	StateKeybindings& forStateNonConst(const std::string& state);

	const std::unordered_map<std::string, StateKeybindings>& getKeybindings() const;
	std::unordered_map<std::string, StateKeybindings>& getKeybindings();

	void restoreDefaultKeybindings();

private:
	std::unordered_map<std::string, StateKeybindings> _keybindings;
	std::unordered_map<std::string, StateKeybindings> _defaultKeybindings;
};

