#pragma once

struct SolidMaterialComponent;

struct SolidMaterialDescription
{
	const SolidMaterialComponent& component;
	float sharpness = 0.f;
};

struct ForceDistribution
{
	float force1 = 0.f;
	float force2 = 0.f;
};

ForceDistribution calculateMaterialForceDistribution(float force, SolidMaterialDescription solid1, SolidMaterialDescription solid2);