#include "pch.h"
#include "Keybindings.h"
#include <Utility/StandardLogger.h>
#include <Utility/stdHashing.h>

static inline StateKeybindings NullStateKeybinding = {};
static inline StateKeybindings NullStateKeybindingNonConst = {};

void Keybindings::createDefaultsFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("states"))
	{
		auto& states = json["states"];
		for (auto&& state : states.GetObject())
		{
			StateKeybindings newStateBindings;
			newStateBindings.createFromJson(state.value);
			_defaultKeybindings.emplace(state.name.GetString(), std::move(newStateBindings));
		}
	}
	else
	{
		error_logger << "Error: Could not parse keybindings, does not contain 'states' key" << std::endl;
	}
}

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

void Keybindings::saveToJson(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const
{
	using namespace rapidjson;
	if (!json.HasMember("states"))
	{
		json.AddMember("states", Value{ kObjectType }, allocator);
	}

	auto& statesObj = json["states"];
	for (auto&& [stateName, stateKeybinding] : _keybindings)
	{
		Value newVal{ kObjectType };
		auto name = GenericStringRef<char>{ stateName.c_str() };
		statesObj.AddMember(name, newVal, allocator);
		auto& val = statesObj[stateName.c_str()];
			
		stateKeybinding.saveToJson(val, allocator);
	}
}

const StateKeybindings& Keybindings::operator[](const std::string& state) const
{
	return forState(state);
}

const StateKeybindings& Keybindings::forState(const std::string& state) const
{
	if (!_keybindings.contains(state))
	{
		error_logger << "Error: keybindings does not contain state " << state.data() << std::endl;
		error_logger << "--- Please check key_bindings.json" << std::endl;
		return NullStateKeybinding;
	}
	return _keybindings.at(state);
}

StateKeybindings& Keybindings::forStateNonConst(const std::string& state)
{
	if (!_keybindings.contains(state))
	{
		error_logger << "Error: keybindings does not contain state " << state.data() << std::endl;
		error_logger << "--- Please check key_bindings.json" << std::endl;
		return NullStateKeybindingNonConst;
	}
	return _keybindings.at(state);
}

const std::unordered_map<std::string, StateKeybindings>& Keybindings::getKeybindings() const
{
	return _keybindings;
}

std::unordered_map<std::string, StateKeybindings>& Keybindings::getKeybindings()
{
	return _keybindings;
}

void Keybindings::restoreDefaultKeybindings()
{
	_keybindings = _defaultKeybindings;
}
