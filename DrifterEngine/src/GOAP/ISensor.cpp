#include "pch.h"
#include "ISensor.h"
#include "Components/Components.h"

component::AI& drft::goap::ISensor::getAI(entt::handle agent) const
{
    return agent.get<component::AI>();
}
