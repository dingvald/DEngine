#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Components.h"
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/BodyComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/SharpComponent.h"
#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>

#include "Systems/Helpers/ItemDatabase.h"
#include <Systems/Helpers/GetDominantSide.h>

#pragma optimize("", off)

static constexpr int CHANCE_TO_DAMAGE_EQUIPPED_WEAPON = 15;
const float C_CONSTANT = 0.5f;
const float K_CONSTANT = 1.5f;

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
		for (auto& [typeName, damage] : weaponDamageTypes)
		{
			attack.damageTypes[typeName] += damage;
		}
	}
}

std::unordered_map<std::string, int> drft::system::BodyPartSystem::calculateDamageTypesFromHeld(entt::entity attacker)
{
	std::unordered_map<std::string, int> result;
	entt::handle actor = { _registry, attacker };
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(actor));
		auto itemEntity = ItemDatabase::getEntityFromItemID(item);

		const int strength = SkillsSystem::getSkillLevel(SkillId::Strength, actor);
		const int agility = SkillsSystem::getSkillLevel(SkillId::Agility, actor);

		float weight = 1.f;
		int sharpness = 0;

		if (itemEntity != entt::null)
		{
			auto material = _registry.try_get<MaterialComponent>(itemEntity);
			auto sharp = _registry.try_get<SharpComponent>(itemEntity);

			if (material) weight = material->weight;
			if (sharp) sharpness = sharp->sharpness;
		}

		float strengthContibution = ((weight + C_CONSTANT) / (weight + K_CONSTANT)) * strength;
		float agilityContribution = ((K_CONSTANT - C_CONSTANT) / (weight + K_CONSTANT)) * agility;

		const float forceSqaured = strengthContibution + agilityContribution;
		const float force = std::sqrtf(forceSqaured);
		if (sharpness > 0)
		{
			result["slashing"] += force + ((weight + sharpness) / 1.8f);
		}
		else
		{
			result["crushing"] += force + weight;
		}

		SkillsSystem::useSkill(SkillId::Strength, static_cast<int>(std::sqrtf(strengthContibution) * 10.f), actor);
		SkillsSystem::useSkill(SkillId::Agility, static_cast<int>(std::sqrtf(agilityContribution) * 10.f), actor);
	}

	return result;
}
 