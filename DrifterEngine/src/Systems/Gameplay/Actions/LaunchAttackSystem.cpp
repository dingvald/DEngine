#include "pch.h"
#include "LaunchAttackSystem.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/SpawnEffect.h"

void drft::system::LaunchAttackSystem::init()
{
}

void drft::system::LaunchAttackSystem::update(const float dt)
{
	auto attackerView = registry->view<component::action::LaunchAttack, component::Attacker, component::tag::Active>();
	for (auto [entity, attack, attacker] : attackerView.each())
	{
		attack.damageTypes["crushing"] += attacker.baseDamage;
	}

	auto launchAttackView = registry->view<component::action::LaunchAttack, component::Position, component::tag::Active>();
	for (auto [entity, attack, pos] : launchAttackView.each())
	{
		const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
		sf::Vector2i targetPosition = pos.position + attack.direction;

		const auto targets = grid.entitiesAt(targetPosition,
			[this](entt::entity entity) -> bool
			{
				if (auto physical = registry->try_get<component::Physical>(entity))
				{
					if (physical->blocks) return true;
				}
				return false;
			});

		if (targets.empty()) continue;
		for (auto target : targets)
		{
			registry->emplace_or_replace<component::action::IncomingDamage>(target, attack.damageTypes, entity);
		}
		std::vector<unsigned int> sprites = { 8,9 }; // crushing
		if (attack.damageTypes.contains("slashing") || attack.damageTypes.contains("piercing"))
		{
			sprites = { 16, 17 }; // slashing
		}

		spawnEffect(*registry, {
			.color = sf::Color::White,
			.sprites = sprites,
			.position = targetPosition,
			.animationSpeed = 20.0f
			});
		spendActionPoints(BASE_ACTION_COST, ActionType::Act, { *registry, entity });
		// HACKZ: Should it care about projectiles? No..
		registry->remove<component::Projectile>(entity);
	}
}

void drft::system::LaunchAttackSystem::onUpdateEnd()
{
	registry->clear<component::action::LaunchAttack>();
}
