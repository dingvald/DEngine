#pragma once



namespace drft::goap
{
	enum WorldStateType : int;

	class WorldState
	{
	public:
		WorldState() = default;
		WorldState(std::initializer_list<std::pair<WorldStateType, int>> init_list);
		int& operator[](WorldStateType key);
		// Adds a new state and value;
		void add(WorldStateType key, int value);
		bool isSameAs(const WorldState& otherState) const;
		bool contains(const WorldState& otherState) const;
		// Returns the number of differences between this state and the other state.
		int distance(const WorldState& otherState) const;
		// Merges the other state into this state.
		// Note: will replace values with the same key.
		void merge(const WorldState& otherState);
		bool isEmpty() const;
	private:
		std::unordered_map<WorldStateType, int> _state;

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


