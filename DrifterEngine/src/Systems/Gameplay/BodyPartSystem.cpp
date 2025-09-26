#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Actions/MeleeAttackAction.h"
#include <Components/Wrappers/BodyWrapper.h>

#pragma optimize("", off)


void drft::system::BodyPartSystem::init()
{
	_registry.on_construct<MeleeAttackAction>().connect<&BodyPartSystem::onMeleeAttackActionAdded>(this);
}

void drft::system::BodyPartSystem::onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity)
{
	entt::handle handle = { registry, entity };
	BodyWrapper body = { handle };
	if (!body.isValid()) return;

	entt::const_handle itemInHand = body.getItemInDominantHand();

	handle.patch<MeleeAttackAction>([itemInHand](MeleeAttackAction& action) {action.itemUsed = itemInHand;});
}
 