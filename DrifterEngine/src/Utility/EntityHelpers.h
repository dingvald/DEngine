#pragma once
namespace drft::util
{
	std::string getEntityName(const entt::const_handle entity);
	bool isType(entt::const_handle entity, const std::string& typeName);
	int getDistanceBetween(entt::const_handle e1, entt::const_handle e2);
}
