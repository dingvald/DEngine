#include "pch.h"
#include "BodyPartSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Utility/ItemIDToEntityID.h"
#include "Random/RandomNumberGenerator.h"

static constexpr int CHANCE_TO_DAMAGE_EQUIPPED_WEAPON = 30;

void drft::system::BodyPartSystem::init()
{
}

void drft::system::BodyPartSystem::update(const float dt)
{
	auto attackView = registry->view<component::Body, component::action::LaunchAttack, component::tag::Active>();
	for (auto [entity, body, attack] : attackView.each())
	{
		if (body.parts.contains("HeldR"))
		{
			const int weaponDamage = calculateDamageFromEquipped(body.parts.at("HeldR"));
			attack.damage += weaponDamage;
		}
	}

	auto damageView = registry->view<component::Body, component::action::TakeDamage, component::tag::Active>();
	for (auto [entity, body, damage] : damageView.each())
	{
		if (damage.amount <= 0) continue;
		auto& part = determinePartHit(body.parts);
	}
}

int drft::system::BodyPartSystem::calculateDamageFromEquipped(unsigned long itemID)
{
	if (itemID != component::Item::NONE)
	{
		const auto rightHandItem = util::ItemIDToEntityID(itemID, *registry);
		float weight = 0.0f;
		float sharpness = 1.f;

		if (auto physicalComp = registry->try_get<component::Physical>(rightHandItem))
		{
			weight += physicalComp->weight;
		}
		if (auto sharpComp = registry->try_get<component::Sharp>(rightHandItem))
		{
			sharpness += std::powf(sharpComp->sharpness, 2.f) * 0.5f;
		}
		if (auto healthComp = registry->try_get<component::Health>(rightHandItem))
		{
			const int roll = rng::RandomNumberGenerator::intInRange(0, 100);
			if (roll < CHANCE_TO_DAMAGE_EQUIPPED_WEAPON)
			{
				registry->emplace<component::action::TakeDamage>(rightHandItem, 1);
			}
		}

		return static_cast<int>(std::ceil(weight * sharpness));
	}
	return 0;
}

std::string& drft::system::BodyPartSystem::determinePartHit(std::unordered_map<std::string, unsigned long>& parts)
{
	std::unordered_map<int, std::string> relativeSize;
	int sum = 0;
	for (auto [part, item] : parts)
	{
		if (part.compare("Body") == 0)
		{
			relativeSize[++sum] = part;
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
