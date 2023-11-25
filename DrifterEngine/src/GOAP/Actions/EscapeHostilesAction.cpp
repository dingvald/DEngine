#include "pch.h"
#include "EscapeHostilesAction.h"

drft::goap::EscapeHostilesAction::EscapeHostilesAction()
{
	addPrecondition("sees_hostile", true);

	addEffect("sees_hostile", false);
}

drft::goap::ActionResult drft::goap::EscapeHostilesAction::perform(entt::handle agent) const
{
	return ActionResult::Continue;
}

int drft::goap::EscapeHostilesAction::cost() const
{
	return 1;
}

bool drft::goap::EscapeHostilesAction::requiresInRange() const
{
	return false;
}
