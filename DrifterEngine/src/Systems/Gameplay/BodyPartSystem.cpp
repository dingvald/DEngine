#include "pch.h"
#include "BodyPartSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Random/RandomNumberGenerator.h"

static constexpr int CHANCE_TO_DAMAGE_EQUIPPED_WEAPON = 30;

void drft::system::BodyPartSystem::init()
{
	registry->on_construct<component::action::IncomingDamage>().connect<&BodyPartSystem::onIncomingDamage>(this);
	registry->on_construct<component::action::LaunchAttack>().connect<&BodyPartSystem::onLaunchAttack>(this);

	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::ItemBreakEvent>().connect<&BodyPartSystem::onItemBreakEvent>(this);
}

void drft::system::BodyPartSystem::onIncomingDamage(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<component::Body>(entity))
	{
		auto& incomingDamage = registry.get<component::action::IncomingDamage>(entity);
		if (incomingDamage.damageTypes.empty()) return;

		const auto& partHit = determinePartHit({registry, entity});
		auto mitigatedDamage = calculateMitigationFromWorn(entity, partHit, incomingDamage.damageTypes);
		incomingDamage.damageTypes = mitigatedDamage;
	}
}

void drft::system::BodyPartSystem::onLaunchAttack(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<component::Body>(entity))
	{
		auto& attack = registry.get<component::action::LaunchAttack>(entity);
		auto weaponDamageTypes = calculateDamageTypesFromHeld(entity);
		for (auto& [typeName, damage] : weaponDamageTypes)
		{
			attack.damageTypes[typeName] += damage;
		}
	}
}

void drft::system::BodyPartSystem::onItemBreakEvent(events::ItemBreakEvent& ev)
{
	auto& body = registry->get<component::Body>(ev.owner);
	body.parts.unequipItem(ev.itemID);
}

std::unordered_map<std::string, int> drft::system::BodyPartSystem::calculateDamageTypesFromHeld(entt::entity attacker)
{
	std::unordered_map<std::string, int> result;
	if (auto body = registry->try_get<component::Body>(attacker))
	{
		const auto rightHand = body->parts.search("Right Hand"); // TODO: genericize
		if (rightHand)
		{
			auto optionalHeld = rightHand->getSlotItem(EquipmentLayer::Held);
			auto itemEntity = ItemDatabase::getEntityFromItemID(optionalHeld.value_or(0));
			if (itemEntity != entt::null)
			{
				if (auto physical = registry->try_get<component::Physical>(itemEntity))
				{
					result["crushing"] += physical->weight;
				}
				if (auto sharp = registry->try_get<component::Sharp>(itemEntity))
				{
					result["slashing"] += sharp->sharpness;
				}
			}

		}
	}

	return result;
}

std::unordered_map<std::string, int> drft::system::BodyPartSystem::calculateMitigationFromWorn(entt::entity defender, const BodyPart& partHit, const std::unordered_map<std::string, int> incomingDamageTypes)
{
	std::unordered_map<std::string, int> result;
	auto itemsEquipped = partHit.getAllCoveringItems();
	for (auto item : itemsEquipped)
	{
		auto itemEntity = ItemDatabase::getEntityFromItemID(item);
		if (itemEntity != entt::null)
		{
			if (auto wearable = registry->try_get<component::Wearable>(itemEntity))
			{
				for (auto& [damageType, amount] : incomingDamageTypes)
				{
					result[damageType] = amount;
					if (wearable->protections.contains(damageType))
					{
						result[damageType] = std::max(0, result[damageType] - wearable->protections.at(damageType));
					}
				}

				if (auto health = registry->try_get<component::Health>(itemEntity))
				{
					// TODO: add damage to the wearable
				}
			}
		}
	}



	

	return result;
}

const BodyPart& drft::system::BodyPartSystem::determinePartHit(entt::handle entity)
{
	BodyPart partHit;
	if (auto body = entity.try_get<component::Body>())
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

 