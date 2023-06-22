#include "pch.h"
#include "LevelingSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"

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
			leveling->neededXP *= 2; // TODO: balance leveling speed
			std::cout << util::getEntityName({ registry, entity }) << " is now level " << leveling->currentLevel << std::endl;
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
}
