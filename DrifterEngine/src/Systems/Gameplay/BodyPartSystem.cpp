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
#include "Random/RandomNumberGenerator.h"
#include "Random/PercentChance.h"
#include "Utility/EntityHelpers.h"

static constexpr int CHANCE_TO_DAMAGE_EQUIPPED_WEAPON = 15;

void drft::system::BodyPartSystem::init()
{
	_registry.on_construct<component::action::IncomingDamage>().connect<&BodyPartSystem::onIncomingDamage>(this);
	_registry.on_construct<MeleeAttackAction>().connect<&BodyPartSystem::onMeleeAttackActionAdded>(this);

	_dispatcher.sink<events::ItemBreakEvent>().connect<&BodyPartSystem::onItemBreakEvent>(this);
}

void drft::system::BodyPartSystem::onIncomingDamage(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<BodyComponent>(entity))
	{
		auto& incomingDamage = registry.get<component::action::IncomingDamage>(entity);
		if (incomingDamage.damageTypes.empty()) return;
		
		const auto& partHit = determinePartHit({registry, entity});

		const auto sourceName = util::getEntityName({ registry, incomingDamage.source });
		const auto targetName = util::getEntityName({ registry, entity });
		std::cout << sourceName << " attacks " << targetName << "'s " << partHit.name << "!" << std::endl;
		auto mitigatedDamage = calculateMitigationFromWorn(entity, partHit, incomingDamage.damageTypes);
		incomingDamage.damageTypes = mitigatedDamage;
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

void drft::system::BodyPartSystem::onItemBreakEvent(events::ItemBreakEvent& ev)
{
	auto& body = _registry.get<BodyComponent>(ev.owner);
	body.parts.unequipItem(ev.itemID);
}

std::unordered_map<std::string, int> drft::system::BodyPartSystem::calculateDamageTypesFromHeld(entt::entity attacker)
{
	std::unordered_map<std::string, int> result;
	if (auto body = _registry.try_get<BodyComponent>(attacker))
	{
		if (const auto rightHand = body->parts.search("Right Hand")) // TODO: Use preferred hand - don't hard code right hand
		{
			auto optionalHeld = rightHand->getSlotItem(EquipmentLayer::Held);
			auto itemEntity = ItemDatabase::getEntityFromItemID(optionalHeld.value_or(ItemComponent::NONE));
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

			if (rng::percentChance(CHANCE_TO_DAMAGE_EQUIPPED_WEAPON))
			{
				component::action::TakeDamage damage{ .amount = 1, .source = entt::null };
				_registry.emplace_or_replace<component::action::TakeDamage>(itemEntity, damage);
			}
		}
	}

	return result;
}

std::unordered_map<std::string, int> drft::system::BodyPartSystem::calculateMitigationFromWorn(entt::entity defender, const BodyPart& partHit, const std::unordered_map<std::string, int> incomingDamageTypes)
{
	std::unordered_map<std::string, int> result = incomingDamageTypes;
	auto itemsEquipped = partHit.getAllSlottedExcept({ EquipmentLayer::Held });
	for (auto item : itemsEquipped)
	{
		auto itemEntity = ItemDatabase::getEntityFromItemID(item);
		if (itemEntity != entt::null)
		{
			if (auto wearable = _registry.try_get<WearableComponent>(itemEntity))
			{
				int sum = 0;
				for (auto& [damageType, amount] : result)
				{
					if (wearable->protections.contains(damageType))
					{
						sum += wearable->protections.at(damageType);
						amount = std::max(0, amount - wearable->protections.at(damageType));
					}
				}

				if (_registry.all_of<HealthComponent>(itemEntity))
				{
					component::action::TakeDamage damage{ .amount = sum, .source = entt::null };
					_registry.emplace_or_replace<component::action::TakeDamage>(itemEntity, damage);
				}
			}
		}
	}

	return result;
}

const BodyPart& drft::system::BodyPartSystem::determinePartHit(entt::handle entity)
{
	BodyPart partHit;
	if (auto body = entity.try_get<BodyComponent>())
	{
		int sum = 0;
		auto partsVector = body->parts.flatten();
		for (auto&& part : partsVector)
		{
			sum += part->size;
		}
		const int choice = rng::RandomNumberGenerator::intInRange(0, sum);
		auto rd = std::random_device{};
		auto rng = std::default_random_engine{rd()};
		std::shuffle(begin(partsVector), end(partsVector), rng);
		for (auto&& part : partsVector)
		{
			sum -= part->size;
			if (sum <= choice)
			{
				return *part;
			}
		}
	}

	return partHit;
}

 