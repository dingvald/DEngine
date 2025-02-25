#include "pch.h"
#include "Keybindings.h"
#include <Utility/StandardLogger.h>

static inline StateKeybindings NullStateKeybinding = {};
static inline StateKeybindings NullStateKeybindingNonConst = {};

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

const StateKeybindings& Keybindings::operator[](entt::hashed_string state) const
{
	return forState(state);
}

const StateKeybindings& Keybindings::forState(entt::hashed_string state) const
{
	if (!_keybindings.contains(state))
	{
		error_logger << "Error: keybindings does not contain state " << state.data() << std::endl;
		error_logger << "--- Please check key_bindings.json" << std::endl;
		return NullStateKeybinding;
	}
	return _keybindings.at(state);
}

StateKeybindings& Keybindings::forStateNonConst(entt::hashed_string state)
{
	if (!_keybindings.contains(state))
	{
		error_logger << "Error: keybindings does not contain state " << state.data() << std::endl;
		error_logger << "--- Please check key_bindings.json" << std::endl;
		return NullStateKeybindingNonConst;
	}
	return _keybindings.at(state);
}
