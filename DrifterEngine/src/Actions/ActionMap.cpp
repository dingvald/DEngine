#include "pch.h"
#include "ActionMap.h"
#include <Keybindings/Keybindings.h>
#include <Keybindings/ModifiedKey.h>
#include <Utility/StandardLogger.h>

void ActionMap::setKeybindings(Keybindings& keybindings)
{
	_keybindings = &keybindings;
}

void ActionMap::bind(const std::string& domain, const std::string& state, const std::string& name, Action action)
{
	if (!_keybindings)
	{
		error_logger << "Error: trying to bind action before keybindings are set" << std::endl;
		return;
	}
	if (!_keybindings->containsState(state))
	{
		error_logger << "Error: trying to bind action to unknown state" << std::endl;
		return;
	}

	_actionMap[domain][state][name] = action;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const std::string& actionName)
{
	auto& map = _actionMap.at(domain).at(state);
	if (!map.contains(actionName)) return false;

	auto& action = std::get<BasicAction>(map.at(actionName));
	action();
	return true;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const std::string& actionName, ArgType arg)
{
	auto& map = _actionMap.at(domain).at(state);
	if (!map.contains(actionName)) return false;

	auto& action = std::get<EntityAction>(map.at(actionName));
	action(arg);
	return true;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const ModifiedInput& input)
{
	if (auto actionName = _keybindings->forState(state).getActionForKey(input))
	{
		auto& map = _actionMap.at(domain).at(state);
		if (!map.contains(actionName.value())) return false;

		auto& action = std::get<BasicAction>(map.at(actionName.value()));
		action();
		return true;
	}
	return false;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const ModifiedInput& input, ArgType arg)
{
	if (auto actionName = _keybindings->forState(state).getActionForKey(input))
	{
		auto& map = _actionMap.at(domain).at(state);
		if (!map.contains(actionName.value())) return false;

		auto& action = std::get<EntityAction>(map.at(actionName.value()));
		action(arg);
		return true;
	}
	return false;
}
