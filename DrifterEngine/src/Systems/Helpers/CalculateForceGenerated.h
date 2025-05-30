#pragma once

struct GeneratedForce
{
	GeneratedForce() = default;
	GeneratedForce(float fromStrength, float fromAgility);
	float total = 0.f;
	float fromStrength = 0.f;
	float fromAgility = 0.f;
};

GeneratedForce calculateForceGenerated(entt::const_handle actor, entt::const_handle item);