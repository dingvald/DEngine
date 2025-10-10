#include "pch.h"
#include "InputFunctions.h"

#include <Components/HealthComponent.h>
#include <Components/PositionComponent.h>
#include <Components/FactionComponent.h>

#include <Systems/Gameplay/FactionSystem.h>

#include <Spatial/Helpers.h>

using namespace drft::system;

float AiInputFunctions::myHealth(entt::const_handle actor, entt::const_handle target)
{
	if (auto health = actor.try_get<HealthComponent>())
	{
		return health->current / health->max;
	}
	return 0.f;
}

float AiInputFunctions::targetHealth(entt::const_handle actor, entt::const_handle target)
{
	if (auto health = target.try_get<HealthComponent>())
	{
		return health->current / health->max;
	}
	return 0.f;
}

float AiInputFunctions::distanceToTarget(entt::const_handle actor, entt::const_handle target)
{
	static constexpr float MAX_DISTANCE = 15.f;

	auto actorPosition = actor.try_get<PositionComponent>();
	auto targetPosition = target.try_get<PositionComponent>();

	if (actorPosition && targetPosition)
	{
		const float dist = drft::spatial::distance3d(actorPosition->tile, targetPosition->tile);
		return dist / MAX_DISTANCE;
	}
	return 0.f;
}

float AiInputFunctions::targetRelationship(entt::const_handle actor, entt::const_handle target)
{
	auto actorFaction = actor.try_get<FactionComponent>();
	auto targetFaction = target.try_get<FactionComponent>();

	if (actorFaction && targetFaction)
	{
		Closeness closeness = FactionSystem::getRelationship(actorFaction->name, targetFaction->name);

		return static_cast<float>(closeness + FactionSystem::MaxCloseness) 
			/ static_cast<float>(std::abs(FactionSystem::MinCloseness) + FactionSystem::MaxCloseness);
	}
	return 0.5f; // Return a neutral relationship
}
