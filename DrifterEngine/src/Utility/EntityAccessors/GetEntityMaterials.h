#pragma once

namespace drft::util
{
	std::vector<entt::const_handle> getEntityMaterials(entt::const_handle entity);
	std::optional<entt::const_handle> getEntityPrimaryMaterial(entt::const_handle entity);
}