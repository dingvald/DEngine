#pragma once
#include <Components/RenderComponent.h>

namespace drft::util
{
	RenderComponent getRenderData(entt::const_handle entity);
	std::string getEntityName(const entt::const_handle entity);
	bool isType(entt::const_handle entity, const std::string& typeName);
	float getDistanceBetween(entt::const_handle e1, entt::const_handle e2);
}
