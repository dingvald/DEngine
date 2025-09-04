#include <pch.h>

#include "AiActionInstance.h"
#include <AI/AiActions/IAiAction.h>

void AiActionInstance::reset(const IAiAction* resetAction)
{
	action = resetAction;
	target = std::nullopt;
	score = 0.f;
}

bool AiActionInstance::isInRange(entt::const_handle actor) const
{
	if (!action) return false;
	return action->isInRange(actor, target);
}

bool AiActionInstance::isValid(entt::const_handle actor) const
{
	if (!action) return false;
	return action->isValid(actor, target);
}

void AiActionInstance::perform(entt::handle actor) const
{
	if (!action) return;
	action->perform(actor, target);
}
