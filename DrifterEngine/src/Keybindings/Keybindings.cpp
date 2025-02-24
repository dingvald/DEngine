#include "pch.h"
#include "Keybindings.h"
#include <Utility/StandardLogger.h>

void Keybindings::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("states"))
	{
		auto& states = json["states"];
		for (auto&& state : states.GetObject())
		{
			StateKeybindings newStateBindings;
			newStateBindings.createFromJson(state.value);
			_keybindings.emplace(state.name.GetString(), std::move(newStateBindings));
		}
	}
	else
	{
		error_logger << "Error: Could not parse keybindings, does not contain 'states' key" << std::endl;
	}
}

void Keybindings::saveToJson(rapidjson::Value& json) const
{
}

const StateKeybindings& Keybindings::getBindings(const std::string& state) const
{
	return _keybindings.at(state);
}

StateKeybindings& Keybindings::getBindingsNonConst(const std::string& state)
{
	return _keybindings.at(state);
}
