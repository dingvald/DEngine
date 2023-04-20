#include "pch.h"
#include "LaunchAttackSystem.h"
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

	auto launchAttackView = registry->view<component::action::LaunchAttack, component::tag::Active>();
	for (auto [entity, attack] : launchAttackView.each())
	{
		const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
		auto& posComp = registry->get<component::Position>(entity);
		sf::Vector2i targetPosition = spatial::toTileSpace(posComp.position) + attack.direction;

		const auto targets = grid.entitiesAt(targetPosition, spatial::Layer::Blocking);
		for (auto target : targets)
		{
			std::cout << "The " << util::getEntityName({ *registry, entity }) << " attacks the " << util::getEntityName({ *registry, target }) << std::endl;
			registry->emplace_or_replace<component::action::TakeDamage>(target, attack.damage);
		}
		
		int actionCost = util::getActionCost({ *registry, entity }, 100, util::ActionType::Act);
		registry->emplace_or_replace<component::action::SpendPoints>(entity, actionCost);
		registry->remove<component::action::LaunchAttack>(entity);
	}
}
