#include "pch.h"
#include "CalculateForceGenerated.h"
#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>
#include <Components/WeightComponent.h>
#include <Components/SharpComponent.h>

GeneratedForce calculateForceGenerated(entt::const_handle actor, entt::const_handle item)
{
    GeneratedForce result;

	const float strength = static_cast<float>(drft::system::SkillsSystem::getSkillLevel(SkillId::Strength, actor));
	const float agility = static_cast<float>(drft::system::SkillsSystem::getSkillLevel(SkillId::Agility, actor));

	float weight = 0.5f;
	float sharpness = 0.0f;
	if (item)
	{
		auto weightComponent = item.try_get<WeightComponent>();
		if (weightComponent) weight = weightComponent->value;
		auto sharpComponent = item.try_get<SharpComponent>();
		if (sharpComponent) sharpness = sharpComponent->value;
	}

	float strengthContribution = 0.f;
	float agilityContribution = 0.f;

	const float strengthCap = sqrtf(strength);
	if (weight <= strengthCap)
	{
		strengthContribution = strength * (weight / strengthCap);
	}
	else
	{
		strengthContribution = strength * strength / (weight * weight);
	}

	agilityContribution = agility / (weight + 1.0f);

	result.fromStrength = strengthContribution * (1 + 0.5f*sharpness);
	result.fromAgility = agilityContribution * (1 + 0.5f*sharpness);
	result.total = strengthContribution + agilityContribution;

    return result;
}
