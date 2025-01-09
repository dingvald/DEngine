#pragma once

class IAiAction
{
public:
	// Checks if the actor is in range of the target
	// Note: target may be self or null for some actions
	bool isInRange(entt::const_handle actor, entt::const_handle target) const;

	virtual float range(entt::const_handle actor) const = 0;

	// Checks the the actor has the necessary components to perform the action
	virtual bool canPerform(entt::const_handle actor) const = 0;

	// Perform the action
	virtual void perform(entt::const_handle actor, entt::const_handle target) const = 0;
};