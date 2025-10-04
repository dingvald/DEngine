#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Actions/MeleeAttackAction.h"
#include <Components/Wrappers/BodyWrapper.h>


void drft::system::BodyPartSystem::init()
{
	_registry.on_construct<MeleeAttackAction>().connect<&BodyPartSystem::onMeleeAttackActionUpdated>(this);
	_registry.on_update<MeleeAttackAction>().connect<&BodyPartSystem::onMeleeAttackActionUpdated>(this);
}

void drft::system::BodyPartSystem::onMeleeAttackActionUpdated(entt::registry& registry, entt::entity entity)
{
	entt::handle handle = { registry, entity };
	BodyWrapper body = { handle };
	if (!body.isValid()) return;

	entt::const_handle itemInHand = body.getItemInDominantHand();
	MeleeAttackAction& action = handle.get<MeleeAttackAction>();
	action.itemUsed = itemInHand;
}