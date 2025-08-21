#include "pch.h"
#include "IAiAction.h"

#include <Utility/EntityHelpers.h>

bool IAiAction::isInRange(entt::const_handle actor, entt::const_handle target) const
{
    if (actor == target) return true;
    return drft::util::getDistanceBetween(actor, target) <= this->range(actor);
}
