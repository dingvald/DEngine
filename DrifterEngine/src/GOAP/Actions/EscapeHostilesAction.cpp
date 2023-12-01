#include "pch.h"
#include "EscapeHostilesAction.h"

drft::goap::EscapeHostilesAction::EscapeHostilesAction()
{
	addPrecondition(sees_hostile, true);

	addEffect(escape_hostile, true);
}

drft::goap::ActionResult drft::goap::EscapeHostilesAction::perform(entt::handle agent) const
{
	// get all hostiles in range
	std::cout << "Trying to escape hostiles!" << std::endl;

	// Move in direction vector away from 
	return ActionResult::Continue;
}

int drft::goap::EscapeHostilesAction::cost() const
{
	return 1;
}

bool drft::goap::EscapeHostilesAction::isInRange(entt::handle agent) const
{
	return true;
}