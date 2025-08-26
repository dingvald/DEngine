#include "pch.h"
#include "WaitAiAction.h"
#include <Components/Actions/WaitAction.h>

bool WaitAiAction::canPerform(entt::const_handle actor) const
{
    return true;
}

void WaitAiAction::perform(entt::handle actor, entt::const_handle) const
{
    actor.emplace_or_replace<WaitAction>();
}

bool WaitAiAction::isInRange(entt::const_handle, entt::const_handle) const
{
    return true;
}
