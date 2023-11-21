#pragma once

namespace drft::goap
{
	class WorldState
	{
	public:
		WorldState() = default;
		WorldState(std::initializer_list<std::pair<std::string, int>> init_list);
		int& operator[](const std::string key);
		// Adds a new state and value;
		void add(const std::string& key, int value);
		bool matches(const WorldState& otherState) const;
		bool isSubsetOf(const WorldState& otherState) const;
		bool isSupersetOf(const WorldState& otherState) const;
		// Returns the number of differences between this state and the other state.
		int distance(const WorldState& otherState) const;
		// Merges the other state into this state.
		// Note: will replace values with the same key.
		void merge(const WorldState& otherState);
	private:
		std::unordered_map<std::string, int> _state;

		friend class cereal::access;
		template<class Archive>
		void save(Archive& archive) const
		{
			archive(_state);
		}

		template<class Archive>
		void load(Archive& archive)
		{
			archive(_state);
		}
	};
}


