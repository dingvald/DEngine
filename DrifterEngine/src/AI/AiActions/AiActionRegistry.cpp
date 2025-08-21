#include "pch.h"
#include "AiActionRegistry.h"
#include <Utility/StandardLogger.h>

const IAiAction* AiActionRegistry::getAction(entt::id_type id) const
{
    if (!_actions.contains(id))
    {
        LOG_ERROR("Action registry does not contain action id {}", id);
        return nullptr;
    }
    return _actions.at(id).get();
}
