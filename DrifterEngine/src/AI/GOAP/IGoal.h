#pragma once
#include "WorldState.h"
#include "WorldStateTypes.h"

namespace drft::goap
{
	class IGoal
	{
	public:
		const WorldState& desiredState() const { return _state; }
		virtual float utility(entt::const_handle agent) const = 0;

	protected:
		void addDesiredState(WorldStateType key, int val);

	private:
		WorldState _state;
	};
}