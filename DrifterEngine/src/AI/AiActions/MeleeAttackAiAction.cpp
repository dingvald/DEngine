#include "pch.h"
#include "MeleeAttackAiAction.h"

#include <Components/PositionComponent.h>
#include <Components/Actions/MeleeAttackAction.h>



bool MeleeAttackAiAction::canPerform(entt::const_handle actor) const
{
    return true;
}

void MeleeAttackAiAction::perform(entt::handle actor, entt::const_handle target) const
{

}

float MeleeAttackAiAction::range(entt::const_handle actor) const
{
    return 1.0f;
}
