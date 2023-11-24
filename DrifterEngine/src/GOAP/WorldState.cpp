#include "pch.h"
#include "WorldState.h"

drft::goap::WorldState::WorldState(std::initializer_list<std::pair<std::string, int>> init_list)
{
	for (auto&& [key, val] : init_list)
	{
		_state.emplace(key, val);
	}
}

int& drft::goap::WorldState::operator[](const std::string key)
{
	return _state[key];
}

void drft::goap::WorldState::add(const std::string& key, int value)
{
	if (_state.contains(key)) throw std::exception("WorldState already has key.");
	_state[key] = value;
}

bool drft::goap::WorldState::isSameAs(const WorldState& otherState) const
{
	for (auto&& [key, val] : _state)
	{
		if (!otherState._state.contains(key)) return false;
		if (otherState._state.at(key) != val) return false;
	}
	return contains(otherState);
}

bool drft::goap::WorldState::contains(const WorldState& otherState) const
{
	for (auto&& [key, val] : otherState._state)
	{
		if (!_state.contains(key)) return false;
		if (_state.at(key) != val) return false;
	}
	return true;
}

int drft::goap::WorldState::distance(const WorldState& otherState) const
{
	int result = 0;

	for (auto&& [key, val] : otherState._state)
	{
		if (!_state.contains(key))
		{
			++result;
			continue;
		}
		else if (_state.at(key) != val)
		{
			++result;
			continue;
		}
	}

	return result;
}

void drft::goap::WorldState::merge(const WorldState& otherState)
{
	for (auto&& [key, val] : otherState._state)
	{
		_state[key] = val;
	}
}

bool drft::goap::WorldState::isEmpty() const
{
	return _state.empty();
}
