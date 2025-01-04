#pragma once

namespace AiInputFunctions
{
	float MyHealth(entt::const_handle actor, entt::const_handle target);
	
	float TargetHealth(entt::const_handle actor, entt::const_handle target);

	float DistanceToTarget(entt::const_handle actor, entt::const_handle target);

	float TargetRelationship(entt::const_handle actor, entt::const_handle target);
}