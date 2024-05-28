#include "pch.h"
#include "LaunchAttackSystem.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"

#include "Components/Components.h"
#include "Components/AttackerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/ProjectileComponent.h"

#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/SpawnEffect.h"

void drft::system::LaunchAttackSystem::init()
{
}

void drft::system::LaunchAttackSystem::update(const float dt)
{
	auto attackerView = _registry->view<component::action::LaunchAttack, AttackerComponent, component::tag::Active>();
	for (auto [entity, attack, attacker] : attackerView.each())
	{
		attack.damageTypes["crushing"] += attacker.baseDamage;
	}

	auto launchAttackView = _registry->view<component::action::LaunchAttack, PositionComponent, component::tag::Active>();
	for (auto [entity, attack, pos] : launchAttackView.each())
	{
		const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
		sf::Vector2i targetPosition = pos.position + attack.direction;

		const auto targets = grid.entitiesAt(targetPosition,
			[this](entt::entity entity) -> bool
			{
				if (auto material = _registry->try_get<MaterialComponent>(entity))
				{
					if (material->blocks) return true;
				}
				return false;
			});

		if (targets.empty()) continue;
		for (auto target : targets)
		{
			_registry->emplace_or_replace<component::action::IncomingDamage>(target, attack.damageTypes, entity);
		}
		std::vector<unsigned int> sprites = { 8,9 }; // crushing
		if (attack.damageTypes.contains("slashing") || attack.damageTypes.contains("piercing"))
		{
			sprites = { 16, 17 }; // slashing
		}

		spawnEffect(*_registry, {
			.color = sf::Color::White,
			.sprites = sprites,
			.position = targetPosition,
			.animationSpeed = 20.0f
			});
		spendActionPoints(BASE_ACTION_COST, ActionType::Act, { *_registry, entity });
		// HACKZ: Should it care about projectiles? No..
		_registry->remove<ProjectileComponent>(entity);
	}
}

void drft::system::LaunchAttackSystem::onUpdateEnd()
{
	_registry->clear<component::action::LaunchAttack>();
}
