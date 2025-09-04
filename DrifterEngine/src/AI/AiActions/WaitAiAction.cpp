#include "pch.h"
#include "WaitAiAction.h"
#include <Components/Actions/WaitAction.h>

bool WaitAiAction::isValid(entt::const_handle, OptionalTarget) const
{
    return true;
}

void WaitAiAction::perform(entt::handle actor, OptionalTarget) const
{
    actor.emplace_or_replace<WaitAction>();
}

bool WaitAiAction::isInRange(entt::const_handle, OptionalTarget) const
{
    return true;
}
