#include "pch.h"
#include "InputFunctions.h"

#include <Components/HealthComponent.h>
#include <Components/PositionComponent.h>
#include <Components/FactionComponent.h>

#include <Systems/Gameplay/FactionSystem.h>

#include <Spatial/Helpers.h>

constexpr float MAX_DISTANCE = 15.f;

float AiInputFunctions::MyHealth(entt::const_handle actor, entt::const_handle target)
{
	if (auto health = actor.try_get<HealthComponent>())
	{
		return health->current / health->max;
	}
	return 0.f;
}

float AiInputFunctions::TargetHealth(entt::const_handle actor, entt::const_handle target)
{
	if (auto health = target.try_get<HealthComponent>())
	{
		return health->current / health->max;
	}
	return 0.f;
}

float AiInputFunctions::DistanceToTarget(entt::const_handle actor, entt::const_handle target)
{
	auto actorPosition = actor.try_get<PositionComponent>();
	auto targetPosition = target.try_get<PositionComponent>();

	if (actorPosition && targetPosition)
	{
		const float dist = drft::spatial::distance3d(actorPosition->tile, targetPosition->tile);
		return dist / MAX_DISTANCE;
	}
	return 0.f;
}

float AiInputFunctions::TargetRelationship(entt::const_handle actor, entt::const_handle target)
{
	auto actorFaction = actor.try_get<FactionComponent>();
	auto targetFaction = target.try_get<FactionComponent>();

	if (actorFaction && targetFaction)
	{
		using namespace drft::system;

		Closeness closeness = FactionSystem::getCloseness(actorFaction->name, targetFaction->name);
		return static_cast<float>(closeness) / static_cast<float>(FactionSystem::MaxCloseness);
	}
	return 0.f;
}
