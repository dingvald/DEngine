#pragma once
namespace drft::util
{
	std::string getEntityName(const entt::const_handle entity);
	int getDistanceBetween(entt::const_handle e1, entt::const_handle e2);
}
