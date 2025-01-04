#include "pch.h"
#include "AiActionRegistry.h"

void AiActionRegistry::registerAction(entt::id_type id, std::unique_ptr<IAiAction> action)
{
    _actions.emplace(id, std::move(action));
}

const IAiAction& AiActionRegistry::getAction(entt::id_type id) const
{
    return *_actions.at(id);
}
