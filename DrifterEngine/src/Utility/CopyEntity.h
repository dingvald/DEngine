#pragma once

namespace drft::util
{
	void copyEntity(entt::entity to, entt::entity from, entt::registry& registry, bool overwrite = true);
	void copyEntity(entt::entity to, entt::entity from, entt::registry& toRegistry, const entt::registry& fromRegistry);
	void copyEntities(const std::vector<entt::entity>& entities, entt::registry& toRegistry, const entt::registry& fromRegistry);
	void copyEntities(entt::registry& toRegistry, entt::registry& fromRegistry);
}