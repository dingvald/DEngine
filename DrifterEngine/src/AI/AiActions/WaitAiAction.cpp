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

float WaitAiAction::range(entt::const_handle) const
{
    return 0.0f;
}
