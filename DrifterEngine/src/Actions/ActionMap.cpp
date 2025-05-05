#include "pch.h"
#include "ActionMap.h"
#include <Keybindings/Keybindings.h>
#include <Keybindings/ModifiedKey.h>
#include <Utility/StandardLogger.h>


ActionMap::ActionMap(Keybindings& keybindings)
	: _keybindings(keybindings)
{
}

void ActionMap::bind(const std::string& domain, const std::string& state, const std::string& name, Action action)
{
	if (!_keybindings.containsState(state))
	{
		LOG_ERROR("Domain {} is trying to bind action {} to unknown state {}", domain, name, state);
		return;
	}

	_actionMap[domain][state][name] = action;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const std::string& actionName) const
{
	auto& map = _actionMap.at(domain).at(state);
	if (!map.contains(actionName)) return false;

	if (auto actionPtr = std::get_if<BasicAction>(&map.at(actionName)))
	{
		(*actionPtr)();
		return true;
	}
	return false;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const std::string& actionName, ArgType arg) const
{
	auto& map = _actionMap.at(domain).at(state);
	if (!map.contains(actionName)) return false;

	if (auto actionPtr = std::get_if<EntityAction>(&map.at(actionName)))
	{
		(*actionPtr)(arg);
		return true;
	}
	return false;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const ModifiedInput& input) const
{
	if (auto actionName = _keybindings.forState(state).getActionForKey(input))
	{
		auto& map = _actionMap.at(domain).at(state);
		if (!map.contains(actionName.value())) return false;

		if (auto actionPtr = std::get_if<BasicAction>(&map.at(actionName.value())))
		{
			(*actionPtr)();
			return true;
		}
	}
	return false;
}

bool ActionMap::call(const std::string& domain, const std::string& state, const ModifiedInput& input, ArgType arg) const
{
	if (!arg) return false;

	if (auto actionName = _keybindings.forState(state).getActionForKey(input))
	{
		auto& map = _actionMap.at(domain).at(state);
		if (!map.contains(actionName.value())) return false;

		if (auto actionPtr = std::get_if<EntityAction>( &map.at( actionName.value() )))
		{
			(*actionPtr)(arg);
			return true;
		}
	}
	return false;
}
