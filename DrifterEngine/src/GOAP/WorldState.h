#pragma once

namespace drft::goap
{
	class WorldState
	{
	public:
		int& operator[](const std::string key);
		// Adds a new state and value;
		void add(const std::string& key, int value);
		bool matches(const WorldState& otherState) const;
		bool isSubset(const WorldState& otherState) const;
		bool isSuperset(const WorldState& otherState) const;
		// Returns the number of differences between this state and the other state.
		int distance(const WorldState& otherState) const;
		// Merges the other state into this state.
		// Note: will replace values with the same key.
		void merge(const WorldState& otherState);
	private:
		std::unordered_map<std::string, int> _state;
	};
}


