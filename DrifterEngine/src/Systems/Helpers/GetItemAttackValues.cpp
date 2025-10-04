#include "pch.h"
#include "GetItemAttackValues.h"
#include <Components/BaseDamageComponent.h>
#include <Components/WeightComponent.h>
#include <Components/SharpnessComponent.h>

#include <Skills/SkillIds.h>
#include <Systems/Gameplay/SkillsSystem.h>

#pragma optimize("", off)

static const drft::math::Range<float> StrengthModLimits{ 0.5f, 2.0f };
static const drft::math::Range<float> AgilityModLimits{ 0.5f, 2.0f };

namespace Internal
{
    static float calculateStrengthModifier(float itemWeight, int actorStrength)
    {
        const float safeWeight = itemWeight > 0.001f ? itemWeight : 0.001f;

        float multiplier = static_cast<float>(actorStrength) / (safeWeight * safeWeight);

        return StrengthModLimits.clamp(multiplier);
    }

    static float calculateAgilityModifier(float itemWeight, int actorAgility)
    {
        const float safeWeight = itemWeight > 0.001f ? itemWeight : 0.001f;

        float multiplier = static_cast<float>(actorAgility) / (safeWeight * safeWeight);

        return AgilityModLimits.clamp(multiplier);
    }
}

AttackValues getItemAttackValues(entt::const_handle item, entt::const_handle user)
{
    if (!item.registry() || !user.registry()) return {};

    AttackValues result;

    int userStrength = drft::system::SkillsSystem::getSkillLevel(SkillId::Strength, user);
    int userAgility = drft::system::SkillsSystem::getSkillLevel(SkillId::Agility, user);

    auto* baseDamageComponent = item.try_get<BaseDamageComponent>();
    result.baseDamageRange.setMin(baseDamageComponent ? std::round(baseDamageComponent->min) : 1.f);
    result.baseDamageRange.setMax(baseDamageComponent ? std::round(baseDamageComponent->max) : 3.f);

    auto* weightComponent = item.try_get<WeightComponent>();
    const float weight = weightComponent ? weightComponent->value : 0.0f;

    result.strengthModifier = Internal::calculateStrengthModifier(weight, userStrength);
    result.agilityModifier = Internal::calculateAgilityModifier(weight, userAgility);

    auto* sharpness = item.try_get<SharpnessComponent>();
    result.sharpnessModifier = sharpness ? SharpnessLevelMultipliers.at(sharpness->currentLevel) : 0.5f;

    result.modifiedDamageRange.setMin(std::round(result.baseDamageRange.getMin() * result.agilityModifier * result.sharpnessModifier));
    result.modifiedDamageRange.setMax(std::round(result.baseDamageRange.getMax() * result.strengthModifier * result.sharpnessModifier));

    return result;
}