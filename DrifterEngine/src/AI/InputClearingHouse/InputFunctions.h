#pragma once

namespace AiInputFunctions
{
	float myHealth(entt::const_handle actor, entt::const_handle target);
	
	float targetHealth(entt::const_handle actor, entt::const_handle target);

	float distanceToTarget(entt::const_handle actor, entt::const_handle target);

	float targetRelationship(entt::const_handle actor, entt::const_handle target);
}