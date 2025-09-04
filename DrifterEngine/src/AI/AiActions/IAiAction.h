#pragma once
#include <Spatial/TilePosition.h>

class IAiAction
{
public:
	using OptionalTarget = std::optional<drft::TilePosition>;

	virtual bool isInRange(entt::const_handle actor, OptionalTarget target) const = 0;

	// Sanity check to ensure the action can be performed on the target
	virtual bool isValid(entt::const_handle actor, OptionalTarget target) const = 0;

	// Perform the action
	virtual void perform(entt::handle actor, OptionalTarget target) const = 0;
};