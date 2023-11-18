#pragma once
#include "Utility/Math.h"
#include "WorldState.h"

namespace drft::goap
{
	class IAction
	{
	public:
		// Returns true if the entity is a valid target for this action.
		virtual bool isValidTarget(entt::const_handle agent, entt::const_handle potentialTarget) = 0;
		// Perform the action. Returns true if the action was performed.
		virtual bool perform(entt::handle agent, entt::handle target) = 0;


		// Checks if the agent is within the range required to perform the action.
		bool isInRange(sf::Vector2i agentPosition, sf::Vector2i targetPosition) const;
		const WorldState& getPreconditions() const;
		const WorldState& getEffects() const;

	protected:
		// Adds a piece of world state that is required for this action to be performed.
		void addPrecondition(const std::string& key, int val);
		// Adds an effect that occurs as a result of this action being performed.
		void addEffect(const std::string& key, int val);
		// The range from the target that this action requires in order to be performed.
		void setRange(int min, int max);
	
	private:
		math::Range<int> _range;
		WorldState _preconditions;
		WorldState _effects;
	};
}


