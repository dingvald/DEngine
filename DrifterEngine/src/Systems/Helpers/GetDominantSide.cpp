#include "pch.h"
#include "GetDominantSide.h"
#include <Components/DominantSideComponent.h>

std::string drft::util::getDominantSide(entt::const_handle entity)
{
	auto dominant = entity.try_get<DominantSideComponent>();
	return dominant ? dominant->side : "right";
}
