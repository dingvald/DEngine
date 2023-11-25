#pragma once
#include "Utility/Math.h"
#include "GOAP/WorldState.h"

namespace component
{
	struct AI;
}

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

		// Sets the AI's target to move to. Returns false if no target could be found.
		[[nodiscard]] virtual std::optional<sf::Vector2i> setMoveTarget(entt::const_handle agent) const;
		// Perform the action. Returns true if the action was performed.
		virtual ActionResult perform(entt::handle agent) const = 0;
		// How expensive this action is to perform relative to other actions.
		virtual int cost() const = 0;
		// Checks if the AI needs to be in range for this action to be performed.
		virtual bool requiresInRange() const = 0;
		// Checks if the AI is in range to perform the action.
		virtual bool isInRange(entt::const_handle agent) const;
		// Checks if the action is valid given the current world state.
		bool isValid(const WorldState& worldState) const;

		const WorldState& preconditions() const;
		const WorldState& effects() const;

	protected:
		// Adds a piece of world state that is required for this action to be performed.
		void addPrecondition(const std::string& key, int val);
		// Adds an effect that occurs as a result of this action being performed.
		void addEffect(const std::string& key, int val);
		// Generic target finder with selector function.
		entt::entity findTarget(entt::const_handle aiEntity, std::function<bool(entt::const_handle)> selector) const;
		const component::AI& getAI(entt::const_handle aiEntity) const;
		component::AI& getAI(entt::handle aiEntity) const;
	
	private:
		WorldState _preconditions;
		WorldState _effects;
	};
}


