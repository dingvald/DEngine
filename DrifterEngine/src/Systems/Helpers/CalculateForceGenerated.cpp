#include "pch.h"
#include "CalculateForceGenerated.h"
#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>
#include <Components/WeightComponent.h>

GeneratedForce calculateForceGenerated(entt::const_handle actor, entt::const_handle item)
{
    GeneratedForce result;

	const float strength = static_cast<float>(drft::system::SkillsSystem::getSkillLevel(SkillId::Strength, actor));
	const float agility = static_cast<float>(drft::system::SkillsSystem::getSkillLevel(SkillId::Agility, actor));

	float weight = 0.5f;
	if (item)
	{
		auto weightComponent = item.try_get<WeightComponent>();
		if (weightComponent) weight = weightComponent->value;
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

	result.fromStrength = strengthContribution;
	result.fromAgility = agilityContribution;
	result.total = strengthContribution + agilityContribution;

    return result;
}
