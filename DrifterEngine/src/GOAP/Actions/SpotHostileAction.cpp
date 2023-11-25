#include "pch.h"
#include "SpotHostileAction.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"

drft::goap::SpotHostileAction::SpotHostileAction()
{
	addPrecondition("sees_hostile", true);
	addEffect("has_target", false);
}

drft::goap::ActionResult drft::goap::SpotHostileAction::perform(entt::handle agent) const
{
	
	return ActionResult::Complete;
}

int drft::goap::SpotHostileAction::cost() const
{
	return 0;
}

bool drft::goap::SpotHostileAction::requiresInRange() const
{
	return false;
}

