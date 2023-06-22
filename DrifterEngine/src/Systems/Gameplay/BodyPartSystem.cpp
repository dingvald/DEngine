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
}

void drft::system::BodyPartSystem::update(const float dt)
{
	auto attackView = registry->view<component::Body, component::action::LaunchAttack, component::tag::Active>();
	for (auto [entity, body, attack] : attackView.each())
	{
		if (body.parts.contains("HeldR"))
		{
			const int weaponDamage = calculateDamageFromEquipped(entity, body.parts.at("HeldR"));
			attack.damage += weaponDamage;
		}
	}
}

void drft::system::BodyPartSystem::onIncomingDamage(entt::registry& registry, entt::entity entity)
{
	if (auto body = registry.try_get<component::Body>(entity))
	{
		auto& incomingDamage = registry.get<component::action::IncomingDamage>(entity);
		if (incomingDamage.amount <= 0) return;
		auto part = determinePartHit(body->parts);
		std::cout << part + " hit!" << std::endl;
		auto itemHit = body->parts.at(part);
		auto itemEntity = ItemDatabase::getEntityFromItemID(itemHit);
		if (itemEntity != entt::null)
		{
			if (auto wearable = registry.try_get<component::Wearable>(itemEntity))
			{
				incomingDamage.amount = std::clamp(incomingDamage.amount - wearable->protection, 0, incomingDamage.amount);
				if (auto health = registry.try_get<component::Health>(itemEntity))
				{
					registry.emplace<component::action::IncomingDamage>(itemEntity, wearable->protection, wearable->protection);
				}
			}
			
		}
	}
}

int drft::system::BodyPartSystem::calculateDamageFromEquipped(entt::entity attacker, unsigned long itemID)
{
	if (itemID != component::Item::NONE)
	{
		const auto rightHandItem = ItemDatabase::getEntityFromItemID(itemID);
		float weight = 0.0f;
		float power = 0.0f;
		float sharpness = 1.f;

		if (auto physicalComp = registry->try_get<component::Physical>(rightHandItem))
		{
			weight += physicalComp->weight;
		}
		if (auto sharpComp = registry->try_get<component::Sharp>(rightHandItem))
		{
			sharpness += sharpComp->sharpness;
		}
		
		if (auto stats = registry->try_get<component::BaseStats>(attacker))
		{
			float strengthContribution = std::max(0.f, (stats->strength - weight)*weight);
			float agilityContribution = 3.f*(sqrtf(powf(stats->agility, 2.f) / weight));
			power = sqrtf(strengthContribution * agilityContribution);
		}

		if (auto healthComp = registry->try_get<component::Health>(rightHandItem))
		{
			const int roll = rng::RandomNumberGenerator::intInRange(0, 100);
			if (roll < CHANCE_TO_DAMAGE_EQUIPPED_WEAPON)
			{
				registry->emplace<component::action::TakeDamage>(rightHandItem, 1);
			}
		}

		return sqrtf(power * sharpness);
	}
	return 0;
}

std::string drft::system::BodyPartSystem::determinePartHit(std::unordered_map<std::string, unsigned long>& parts)
{
	std::unordered_map<int, std::string> relativeSize;
	int sum = 0;
	for (auto [part, item] : parts)
	{

		if (part.compare("HeldL") == 0)
		{
			// Some type of shield
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			if (registry->all_of<component::Wearable>(itemEntity))
			{
				relativeSize[++sum] = part;
				relativeSize[++sum] = part;
				relativeSize[++sum] = part;
				relativeSize[++sum] = part;
				relativeSize[++sum] = part;
			}
		}
		if (part.compare("Body") == 0)
		{
			relativeSize[++sum] = part;
			relativeSize[++sum] = part;
			relativeSize[++sum] = part;
		}
		else if (part.compare("Legs") == 0)
		{
			relativeSize[++sum] = part;
			relativeSize[++sum] = part;
		}
		else if (part.compare("HeldR") != 0 && part.compare("HeldL") != 0)
		{
			relativeSize[++sum] = part;
		}
	}

	const int selectedPart = rng::RandomNumberGenerator::intInRange(1, sum);
	return relativeSize.at(selectedPart);
}
