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
		if (incomingDamage.amount <= 0) return;

		auto partName = determinePartHit({registry, entity});
		auto itemHit = body->parts.search(partName)->getAllSlotted().back();
		auto itemEntity = ItemDatabase::getEntityFromItemID(itemHit);
		int mitigation = calculateMitigationFromWorn(entity, itemHit);
		incomingDamage.amount = std::clamp(incomingDamage.amount - mitigation, 0, incomingDamage.amount);

		if (itemEntity != entt::null)
		{
			if (auto wearable = registry.try_get<component::Wearable>(itemEntity))
			{
				if (auto health = registry.try_get<component::Health>(itemEntity))
				{
					// TODO: add damage to the wearable
				}
			}
		}
	}
}

void drft::system::BodyPartSystem::onLaunchAttack(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<component::Body>(entity))
	{
		auto& attack = registry.get<component::action::LaunchAttack>(entity);
		attack.damage += calculateForceFromHeld(entity);
	}
}

void drft::system::BodyPartSystem::onItemBreakEvent(events::ItemBreakEvent& ev)
{
	auto& body = registry->get<component::Body>(ev.owner);
	body.parts.unequipItem(ev.itemID);
}

int drft::system::BodyPartSystem::calculateForceFromHeld(entt::entity attacker)
{
	int force = 0;
	if (auto body = registry->try_get<component::Body>(attacker))
	{
		auto rightHand = body->parts.search("Right Hand"); // TODO: genericize
		if (rightHand)
		{
			auto optionalHeld = rightHand->getSlotItem(EquipmentLayer::Held);
			auto itemEntity = ItemDatabase::getEntityFromItemID(optionalHeld.value_or(0));
			if (itemEntity != entt::null)
			{
				if (auto physical = registry->try_get<component::Physical>(itemEntity))
				{
					force += physical->weight;
				}
				if (auto sharp = registry->try_get<component::Sharp>(itemEntity))
				{
					force += sharp->sharpness;
				}
			}

		}
	}

	return std::ceilf(force);
}

int drft::system::BodyPartSystem::calculateMitigationFromWorn(entt::entity defender, unsigned long itemHit)
{
	int result = 0;



	return result;
}

std::string drft::system::BodyPartSystem::determinePartHit(entt::handle entity)
{
	std::string result;
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
				result = part->name;
				return result;
			}
		}
	}

	return result;
}

 