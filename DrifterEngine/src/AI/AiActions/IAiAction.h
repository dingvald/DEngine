#pragma once

class IAiAction
{
public:
	virtual bool isInRange(entt::const_handle actor, entt::const_handle target) const = 0;

	// Checks if the actor has the necessary components to perform the action
	virtual bool canPerform(entt::const_handle actor) const = 0;

	// Perform the action
	virtual void perform(entt::handle actor, entt::const_handle target) const = 0;
};