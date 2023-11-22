#include "pch.h"
#include "SpotHostileAction.h"
#include "Components/Components.h"

drft::goap::SpotHostileAction::SpotHostileAction()
{
	addPrecondition("near_hostile", true);
	addPrecondition("sees_hostile", false);

	addEffect("sees_hostile", true);
}

bool drft::goap::SpotHostileAction::setTarget(entt::const_handle agent) const
{
	return false;
}

bool drft::goap::SpotHostileAction::perform(entt::handle agent, entt::handle target) const
{
	auto& ai = getAI(agent);


	return false;
}

int drft::goap::SpotHostileAction::cost() const
{
	return 1;
}

bool drft::goap::SpotHostileAction::requiresInRange() const
{
	return false;
}
