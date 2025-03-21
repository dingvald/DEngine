#include "pch.h"
#include "BodyPartSystem.h"

#include "Components/Components.h"
#include "Components/Actions/MeleeAttackAction.h"
#include "Components/BodyComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/ItemComponent.h"
#include "Components/SharpComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WearableComponent.h"
#include "Components/Tags.h"

#include "Systems/Helpers/ItemDatabase.h"
#include <Systems/Helpers/GetDominantSide.h>
#include "Random/Random.h"
#include "Random/PercentChance.h"
#include "Utility/EntityHelpers.h"

static constexpr int CHANCE_TO_DAMAGE_EQUIPPED_WEAPON = 15;

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
	entt::const_handle actor = { _registry, attacker };
	if (auto body = actor.try_get<BodyComponent>())
	{
		auto item = body->parts.getEquipped(BodyPart::Slot::Type::Held, util::getDominantSide(actor));
		auto itemEntity = ItemDatabase::getEntityFromItemID(item);

		if (itemEntity != entt::null)
		{
			if (auto material = _registry.try_get<MaterialComponent>(itemEntity))
			{
				result["crushing"] += material->weight;
			}
			if (auto sharp = _registry.try_get<SharpComponent>(itemEntity))
			{
				result["slashing"] += sharp->sharpness;
			}
		}
	}

	return result;
}
 