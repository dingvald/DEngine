#include "pch.h"
#include "SpotHostileAction.h"
#include "Components/Components.h"
#include "Systems/Gameplay/FactionSystem.h"

drft::goap::SpotHostileAction::SpotHostileAction()
{
	addPrecondition("near_hostile", true);

	addEffect("sees_hostile", true);
}

drft::goap::ActionResult drft::goap::SpotHostileAction::perform(entt::handle agent) const
{
	std::cout << "Agent looking for target!" << std::endl;
	auto target = findTarget(agent, [&agent](entt::const_handle handle) -> bool {
		if (auto faction = handle.try_get<component::Faction>())
		{
			return system::FactionSystem::resolveRelationship(agent, handle) == system::Relationship::Hostile;
		}
		return false; });

	if (target == entt::null)
	{
		getAI(agent).blackboard["near_hostile"] = false;
		return ActionResult::Failed;
	}
	std::cout << "Agent found target!" << std::endl;
	getAI(agent).target = target;
	getAI(agent).blackboard["sees_hostile"] = true;
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

