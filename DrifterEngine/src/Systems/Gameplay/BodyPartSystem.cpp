#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Actions/MeleeAttackAction.h"
#include "Components/BodyComponent.h"

#include "Systems/Helpers/ItemDatabase.h"
#include <Systems/Helpers/GetDominantSide.h>

namespace
{
	struct SplitDamage
	{
		float crushing = 1.f;
		float slashing = 0.f;
	};

	SplitDamage determineDamageSplit(float percentSharpness)
	{
		SplitDamage result;
		percentSharpness = std::clamp(percentSharpness, 0.f, 1.f);
		result.slashing = percentSharpness;
		result.crushing = 1.f - percentSharpness;
		return result;
	}
}

void drft::system::BodyPartSystem::init()
{
	_registry.on_construct<MeleeAttackAction>().connect<&BodyPartSystem::onMeleeAttackActionAdded>(this);
}

void drft::system::BodyPartSystem::onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<BodyComponent>(entity))
	{
		auto& attack = registry.get<MeleeAttackAction>(entity);
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide({ registry, entity }));

		auto itemEntity = item != 0ul ? ItemDatabase::getEntityFromItemID(item) : entity;
		
		attack.itemUsed = entt::const_handle{ registry, itemEntity };
	}
}
 