#include "pch.h"
#include "LaunchAttackSystem.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Utility/EntityHelpers.h"

void drft::system::LaunchAttackSystem::init()
{
}

void drft::system::LaunchAttackSystem::update(const float dt)
{
	auto attackerView = registry->view<component::action::LaunchAttack, component::Attacker, component::tag::Active>();
	for (auto [entity, attack, attacker] : attackerView.each())
	{
		attack.damage += attacker.baseDamage;
	}

	auto launchAttackView = registry->view<component::action::LaunchAttack, component::Position, component::tag::Active>();
	for (auto [entity, attack, pos] : launchAttackView.each())
	{
		const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
		sf::Vector2i targetPosition = spatial::toTileSpace(pos.position) + attack.direction;

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
			std::cout << "The " << util::getEntityName({ *registry, entity }) << " attacks the " << util::getEntityName({ *registry, target }) << std::endl;
			registry->emplace_or_replace<component::action::TakeDamage>(target, attack.damage);
		}
		spendActionPoints(*registry, entity, ActionType::Act);
	}
}

void drft::system::LaunchAttackSystem::onUpdateEnd()
{
	registry->clear<component::action::LaunchAttack>();
}
