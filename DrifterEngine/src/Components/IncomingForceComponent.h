#pragma once
#include <EnTT/entity/entity.hpp>

struct IncomingForceComponent
{
	float force = 0.f;
	entt::const_handle entity = {};
};