#pragma once
#include <Keybindings/StateKeybindings.h>


class Keybindings : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void saveToJson(rapidjson::Value& json) const;

	const StateKeybindings& getBindings(const std::string& state) const;
	StateKeybindings& getBindingsNonConst(const std::string& state);

private:
	std::unordered_map<std::string, StateKeybindings> _keybindings;
};

