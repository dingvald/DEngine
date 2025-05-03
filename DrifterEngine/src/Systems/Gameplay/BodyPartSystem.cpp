#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Components.h"
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/BodyComponent.h"
#include "Components/WeightComponent.h"
#include "Components/SharpComponent.h"
#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>

#include "Systems/Helpers/ItemDatabase.h"
#include <Systems/Helpers/GetDominantSide.h>

#pragma optimize("", off)

static constexpr int CHANCE_TO_DAMAGE_EQUIPPED_WEAPON = 15;
const float C_CONSTANT = 0.5f;
const float K_CONSTANT = 1.5f;
static constexpr float SKILL_POINT_MULIPLIER = 10.f;

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
	_registry.on_construct<component::action::IncomingDamage>().connect<&BodyPartSystem::onIncomingDamage>(this);
	_registry.on_construct<MeleeAttackAction>().connect<&BodyPartSystem::onMeleeAttackActionAdded>(this);
}

void drft::system::BodyPartSystem::onIncomingDamage(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<BodyComponent>(entity))
	{
		auto& incomingDamage = registry.get<component::action::IncomingDamage>(entity);
		if (incomingDamage.damageTypes.empty()) return;

		// TODO: Add damage mitigation

	}
}

void drft::system::BodyPartSystem::onMeleeAttackActionAdded(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<BodyComponent>(entity))
	{
		auto& attack = registry.get<MeleeAttackAction>(entity);
		auto weaponDamageTypes = calculateDamageTypesFromHeld(entity);
		
		Damage::mergeOrAdd(attack.damageTypes, weaponDamageTypes);
	}
}

std::unordered_map<std::string, int> drft::system::BodyPartSystem::calculateDamageTypesFromHeld(entt::entity attacker)
{
	Damage::TypeMap result;
	entt::handle actor = { _registry, attacker };
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(actor));
		auto itemEntity = ItemDatabase::getEntityFromItemID(item);

		const int strength = SkillsSystem::getSkillLevel(SkillId::Strength, actor);
		const int agility = SkillsSystem::getSkillLevel(SkillId::Agility, actor);

		float weight = 1.f;
		float sharpness = 0.f;

		if (itemEntity != entt::null)
		{
			auto weightComponent = _registry.try_get<WeightComponent>(itemEntity);
			auto sharp = _registry.try_get<SharpComponent>(itemEntity);

			if (weightComponent) weight = weightComponent->value;
		}

		const float strengthContibution = ((weight + C_CONSTANT) / (weight + K_CONSTANT)) * strength;
		const float agilityContribution = ((K_CONSTANT - C_CONSTANT) / (weight + K_CONSTANT)) * agility;

		const float forceSqaured = strengthContibution + agilityContribution;
		const float force = std::sqrtf(forceSqaured);

		SplitDamage split = determineDamageSplit(sharpness);

		result["slashing"] += std::ceilf((force + weight) * split.slashing);
		result["crushing"] += std::ceilf((force + weight) * split.crushing);

		SkillsSystem::useSkill(SkillId::Strength, (int)(std::sqrtf(strengthContibution) * SKILL_POINT_MULIPLIER), actor);
		SkillsSystem::useSkill(SkillId::Agility, (int)(std::sqrtf(agilityContribution) * SKILL_POINT_MULIPLIER), actor);
	}

	return result;
}
 