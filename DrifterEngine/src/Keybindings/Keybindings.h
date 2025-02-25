#pragma once
#include <Keybindings/StateKeybindings.h>

class Keybindings : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void saveToJson(rapidjson::Value& json) const;

	const StateKeybindings& operator [](entt::hashed_string state) const;

	const StateKeybindings& forState(entt::hashed_string state) const;
	StateKeybindings& forStateNonConst(entt::hashed_string state);

private:
	std::unordered_map<entt::hashed_string, StateKeybindings> _keybindings;
};

