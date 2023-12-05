#include "pch.h"
#include "LevelingSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Events/SendFloatingMessageEvent.h"

void drft::system::LevelingSystem::init()
{
	registry->on_construct<component::action::GainExperience>().connect<&LevelingSystem::onXPGained>(this);
	registry->on_construct<component::action::LevelUp>().connect<&LevelingSystem::onLevelUp>(this);
}

void drft::system::LevelingSystem::onUpdateEnd()
{
	registry->clear<component::action::GainExperience>();
	registry->clear<component::action::LevelUp>();
}

void drft::system::LevelingSystem::onXPGained(entt::registry& registry, entt::entity entity)
{
	if (auto leveling = registry.try_get<component::Leveling>(entity))
	{
		auto xp = registry.get<component::action::GainExperience>(entity);
		leveling->currentXP += xp.amount;
		std::cout << util::getEntityName({ registry, entity }) << " gained " << xp.amount << " xp" << std::endl;
		if (leveling->currentXP >= leveling->neededXP)
		{
			// level up
			++leveling->currentLevel;
			leveling->currentXP -= leveling->neededXP;
			leveling->neededXP *= 2.5; // TODO: balance leveling speed
			registry.emplace<component::action::LevelUp>(entity);
		}
	}
}

void drft::system::LevelingSystem::onLevelUp(entt::registry& registry, entt::entity entity)
{
	if (registry.all_of<component::BaseStats>(entity))
	{
		registry.patch<component::BaseStats>(entity, [](component::BaseStats& stats) {
			stats.strength++;
			stats.agility++;
			stats.vitality++;
			});
	}
	registry.patch<component::action::LevelUp>(entity,
		[](component::action::LevelUp& levelUp)
		{
			levelUp.statChanges.emplace("strength", 1);
			levelUp.statChanges.emplace("agility", 1);
			levelUp.statChanges.emplace("vitality", 1);
		});

	auto& dispatcher = registry.ctx().get<entt::dispatcher&>();
	dispatcher.trigger(events::SendFloatingMessageEvent{
		.message = "LEVEL UP",
		.color = sf::Color::Magenta,
		.tracksEntity = entity,
		.position = registry.get<component::Position>(entity).position,
		.velocity = {0,-0.1},
		.isScreenSpace = false,
		.ttl = 100
		});
}
