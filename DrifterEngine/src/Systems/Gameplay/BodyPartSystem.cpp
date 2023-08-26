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
		if (body.parts.contains("HeldR") || body.parts.contains("HeldL"))
		{
			const int weaponDamage = calculateDamageFromHeld(entity, body.parts.at("HeldR"), body.parts.at("HeldL"));
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
		auto itemHit = body->parts.at(part);
		auto itemEntity = ItemDatabase::getEntityFromItemID(itemHit);
		if (itemEntity != entt::null)
		{
			if (auto wearable = registry.try_get<component::Wearable>(itemEntity))
			{
				if (auto health = registry.try_get<component::Health>(itemEntity))
				{
					registry.emplace<component::action::IncomingDamage>(itemEntity, wearable->protection, wearable->protection);
				}
			}
		}
		int mitigation = calculateMitigationFromWorn(entity, itemHit);
		incomingDamage.amount = std::clamp(incomingDamage.amount - mitigation, 0, incomingDamage.amount);
	}
}

int drft::system::BodyPartSystem::calculateDamageFromHeld(entt::entity attacker, unsigned long itemR, unsigned long itemL)
{
	int result = 0;
	float weight = 1.0f;
	float power = 1.0f;
	float speed = 1.0f;
	float sharpness = 0.f;
	bool isLeftHandEmpty = (itemL == component::Item::NONE);

	if (itemR != component::Item::NONE)
	{
		const auto rightHandItem = ItemDatabase::getEntityFromItemID(itemR);
		if (auto physicalComp = registry->try_get<component::Physical>(rightHandItem))
		{
			weight = physicalComp->weight;
		}
		if (auto sharpComp = registry->try_get<component::Sharp>(rightHandItem))
		{
			sharpness = sharpComp->sharpness;
		}
		if (auto healthComp = registry->try_get<component::Health>(rightHandItem))
		{
			const int roll = rng::RandomNumberGenerator::intInRange(0, 100);
			if (roll < CHANCE_TO_DAMAGE_EQUIPPED_WEAPON)
			{
				registry->emplace<component::action::TakeDamage>(rightHandItem, 1);
			}
		}
	}
	if (auto stats = registry->try_get<component::BaseStats>(attacker))
	{
		float effectiveStrength = isLeftHandEmpty ? stats->strength * 2.f : stats->strength;
		power = std::max(0.f, effectiveStrength * ((-(1 / powf((effectiveStrength), 2.f)) * powf(weight - (effectiveStrength / 2), 2.f) + 1.f)));
		speed = std::min(static_cast<float>(stats->agility), std::max(0.f, (2.f * stats->agility) / weight));
	}

	float maxDamage = std::ceil(sqrtf(speed * power * weight) + powf(sharpness, 1.5f));
	float minDamage = std::min(maxDamage, std::floorf(sqrtf(speed * weight) + powf(sharpness, 2.f)));
	float damage = rng::RandomNumberGenerator::realInRange(minDamage, maxDamage);

	return std::ceilf(damage);
}

int drft::system::BodyPartSystem::calculateMitigationFromWorn(entt::entity defender, unsigned long partHit)
{
	float sum = 0.f;
	float fromHit = 0.f;
	int count = 0;
	if (auto body = registry->try_get<component::Body>(defender))
	{
		for (auto&& [partName, itemID] : body->parts)
		{
			++count;
			auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);
			if (auto wearable = registry->try_get<component::Wearable>(itemEntity))
			{
				if (itemID == partHit)
				{
					fromHit = wearable->protection;
				}
				sum += wearable->protection;
			}
		}
	}
	count = std::max(1, count);
	float average = sum / count;
	return static_cast<int>(std::ceil(average + fromHit));
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
