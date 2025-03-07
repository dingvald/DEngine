#pragma once

#include <Keybindings/Keybindings.h>

struct ModifiedInput;

class ActionMap
{
public:
	using BasicAction = std::function<void(void)>;
	using ArgType = entt::handle;
	using EntityAction = std::function<void(ArgType)>;
	using Action = std::variant<BasicAction, EntityAction>;

public:
	void setKeybindings(Keybindings& keybindings);

	void bind(const std::string& domain, const std::string& state, const std::string& name, Action action);

	bool call(const std::string& domain, const std::string& state, const std::string& actionName);
	bool call(const std::string& domain, const std::string& state, const std::string& actionName, ArgType arg);

	bool call(const std::string& domain, const std::string& state, const ModifiedInput& input);
	bool call(const std::string& domain, const std::string& state, const ModifiedInput& input, ArgType arg);

private:
	using NameActionMap = std::unordered_map<std::string, Action>;
	using StateNameToActionMap = std::unordered_map<std::string, NameActionMap>;

	std::unordered_map<std::string, StateNameToActionMap> _actionMap;
	Keybindings* _keybindings = nullptr;
};