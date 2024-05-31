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
	_registry->on_construct<component::action::LaunchAttack>().connect<&LaunchAttackSystem::onLaunchAttackAction>(this);
}

void drft::system::LaunchAttackSystem::update(const float dt)
{
	auto launchAttackView = _registry->view<component::action::LaunchAttack, PositionComponent, component::tag::Active>();
	for (auto [entity, attack, pos] : launchAttackView.each())
	{
		for (auto target : attack.targets)
		{
			_registry->emplace_or_replace<component::action::IncomingDamage>(target, attack.damageTypes, entity);
		}

		std::vector<unsigned int> sprites = { 8,9 }; // crushing
		if (attack.damageTypes.contains("slashing") || attack.damageTypes.contains("piercing"))
		{
			sprites = { 16, 17 }; // slashing
		}

		const sf::Vector2i targetPosition = pos.position + attack.direction;
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

void drft::system::LaunchAttackSystem::onLaunchAttackAction(entt::registry& registry, entt::entity entity) const
{
	if (auto attackerComponent = registry.try_get<AttackerComponent>(entity))
	{
		auto& launchAttackComponent = registry.get<component::action::LaunchAttack>(entity);
		launchAttackComponent.damageTypes["crushing"] += attackerComponent->baseDamage;
	}
}
