#include "pch.h"
#include "MaterialForceDistribution.h"
#include <Components/SolidMaterialComponent.h>

// Solid Material Property Scalars
static const float HardnessScalar = 4.0f;
static const float DensityScalar = 3.0f;
static const float BrittlenessScalar = 2.0f;
static const float MalleabilityScalar = 1.0f;
static const float SharpnessScalar = 2.666f;
static const float SolidScalarSum = HardnessScalar + DensityScalar + BrittlenessScalar + MalleabilityScalar;

namespace Internal
{
	static float calculateMaterialScore(const SolidMaterialDescription& solid, float densityRatio)
	{
		float hardness = solid.component.hardness * HardnessScalar;
		float density = (densityRatio + (densityRatio * solid.sharpness * SharpnessScalar)) * DensityScalar; // Sharpness positively modifies density
		float brittleness = (1.0f - solid.component.brittleness) * BrittlenessScalar; // Lower brittleness = better
		float malleability = (1.0f - solid.component.malleability) * MalleabilityScalar; // Lower malleability = better
		return (hardness + density + brittleness + malleability) / SolidScalarSum;
	}
}

ForceDistribution calculateMaterialForceDistribution(float force, SolidMaterialDescription solid1, SolidMaterialDescription solid2)
{
	const float epsilon = FLT_EPSILON; // Avoid division by zero

	// Calculate relative density contributions (normalized to 0-1 range)
	float total_density = solid1.component.density + solid2.component.density + epsilon;
	float attack_density_ratio = solid2.component.density / total_density;  // Attacker's contribution
	float defense_density_ratio = solid1.component.density / total_density;  // Defender's contribution

	// Attack score (solid2's ability to deal damage)
	float attackScore = Internal::calculateMaterialScore(solid2, attack_density_ratio);
	float defenseScore = Internal::calculateMaterialScore(solid1, defense_density_ratio);

	// Calculate force distribution (attack vs. defense)
	float total = std::max(attackScore + defenseScore, epsilon);
	float distribution = attackScore / total;

	return ForceDistribution{ distribution * force, (1.f - distribution) * force };
}
