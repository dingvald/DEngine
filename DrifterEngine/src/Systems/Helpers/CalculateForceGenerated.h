#pragma once

struct GeneratedForce
{
	float total = 0.f;
	float fromStrength = 0.f;
	float fromAgility = 0.f;
};

GeneratedForce calculateForceGenerated(entt::const_handle actor, entt::const_handle item);