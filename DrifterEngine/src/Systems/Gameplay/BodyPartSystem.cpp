#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Actions/MeleeAttackAction.h"
#include "Components/BodyComponent.h"
#include <Components/Wrappers/BodyWrapper.h>

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
	BodyWrapper body = { entt::handle{registry, entity} };
	if (!body.isValid()) return;

	auto itemInHand = body.getItemInDominantHand();
	auto& attack = registry.get<MeleeAttackAction>(entity);
	attack.itemUsed = itemInHand;
}
 