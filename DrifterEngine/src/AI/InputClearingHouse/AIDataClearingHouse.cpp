#include "pch.h"
#include "AIDataClearingHouse.h"

#include <Utility/StandardLogger.h>

AIDataClearingHouse::AIDataClearingHouse(entt::registry& registry)
    : _registry(registry)
{}

float AIDataClearingHouse::getInput(UtilityInputID inputID, entt::entity actor, entt::entity target) const
{
    if (_inputFunctions.contains(inputID))
    {
        entt::const_handle actorHandle = { _registry, actor };
        entt::const_handle targetHandle = { _registry, target };

        return _inputFunctions.at(inputID)(actorHandle, targetHandle);
    }
    else
    {
        warning_logger << "Warning: input id missing from AI clearning house - returning 0.0" << std::endl;
    }
    return 0.f;
}

void AIDataClearingHouse::registerInput(UtilityInputID id, InputProviderFunction func)
{
    _inputFunctions.emplace(id, func);
}
