#pragma once
#include "Utility/Math.h"
#include "GOAP/WorldState.h"
#include "GOAP/WorldStateTypes.h"

struct AIComponent;

namespace drft::goap
{
	enum class ActionResult
	{
		Complete,
		Continue,
		Failed,
		Error
	};

	class IAction
	{
	public:
		IAction() = default;
		IAction(IAction&) = delete;
		IAction& operator=(IAction&) = delete;
		virtual ~IAction() = default;

		// Called when the action first hits the top of the action queue.
		virtual void onStart(entt::handle agent) const {};
		// Sets the AI's target to move to. Returns true if a target location was found.
		virtual std::optional<sf::Vector2i> trySetTarget(entt::handle agent) const;
		// Perform the action. Returns true if the action was performed.
		virtual ActionResult perform(entt::handle agent) const = 0;
		// How expensive this action is to perform relative to other actions.
		virtual int cost() const = 0;
		// Checks if the AI is in range to perform the action.
		virtual bool isInRange(entt::handle agent) const = 0;
		// Checks if the action is valid given the current world state.
		bool isValid(const WorldState& worldState) const;

		const WorldState& preconditions() const;
		const WorldState& effects() const;

	protected:
		// Adds a piece of world state that is required for this action to be performed.
		void addPrecondition(WorldStateType state, int val);
		// Adds an effect that occurs as a result of this action being performed.
		void addEffect(WorldStateType state, int val);
		const AIComponent& getAI(entt::const_handle aiEntity) const;
		AIComponent& getAI(entt::handle aiEntity) const;
	
	private:
		WorldState _preconditions;
		WorldState _effects;
	};
}


