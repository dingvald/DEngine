#include "pch.h"
#include "LevelingSystem.h"

#include "Components/Components.h"
#include "Components/LevelingComponent.h"
#include "Components/PositionComponent.h"

#include "Utility/EntityHelpers.h"
#include "Events/SendFloatingMessageEvent.h"
#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>

void drft::system::LevelingSystem::init()
{
	_registry.on_construct<component::action::GainExperience>().connect<&LevelingSystem::onXPGained>(this);
	_registry.on_construct<component::action::LevelUp>().connect<&LevelingSystem::onLevelUp>(this);
}

void drft::system::LevelingSystem::updateEnd()
{
	_registry.clear<component::action::GainExperience>();
	_registry.clear<component::action::LevelUp>();
}

void drft::system::LevelingSystem::onXPGained(entt::registry& registry, entt::entity entity)
{
	if (auto leveling = registry.try_get<LevelingComponent>(entity))
	{
		auto& xp = registry.get<component::action::GainExperience>(entity);
		leveling->currentXP += xp.amount;
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
	_dispatcher.trigger(events::SendFloatingMessageEvent{
		.message = "LEVEL UP",
		.color = sf::Color::Magenta,
		.tracksEntity = entity,
		.position = spatial::toXY(spatial::toFloatSpace(registry.get<PositionComponent>(entity).tile)),
		.velocity = {0,-0.1},
		.isScreenSpace = false,
		.ttl = 100
		});
}
