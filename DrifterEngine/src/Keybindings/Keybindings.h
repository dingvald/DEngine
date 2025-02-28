#pragma once
#include <Keybindings/StateKeybindings.h>

class Keybindings : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void saveToJson(rapidjson::Value& json) const;

	const StateKeybindings& operator [](const std::string& state) const;

	const StateKeybindings& forState(const std::string& state) const;
	StateKeybindings& forStateNonConst(const std::string& state);

	const std::unordered_map<std::string, StateKeybindings>& getKeybindings() const;
	std::unordered_map<std::string, StateKeybindings>& getKeybindings();

private:
	std::unordered_map<std::string, StateKeybindings> _keybindings;
};

