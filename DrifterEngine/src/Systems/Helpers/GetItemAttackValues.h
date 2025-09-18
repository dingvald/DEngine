#pragma once
#include <Utility/Math.h>

struct AttackValues
{
	drft::math::Range<float> baseDamageRange{ 0.f, 1.f };
	drft::math::Range<float> modifiedDamageRange{ 0.f, 1.f };

	float sharpnessModifier = 0.f;
	float strengthModifier = 1.f;
	float agilityModifier = 1.f;
};

AttackValues getItemAttackValues(entt::const_handle item, entt::const_handle user);