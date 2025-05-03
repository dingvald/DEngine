#pragma once
#include <Components/RenderComponent.h>

namespace drft::util
{
	RenderComponent getRenderData(entt::const_handle entity);
	bool isType(entt::const_handle entity, entt::id_type typeId);
	float getDistanceBetween(entt::const_handle e1, entt::const_handle e2);
}
