#include <pch.h>
#include "HealthSystem.h"

#include "Components/Components.h"
#include "Components/HealthComponent.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"

#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/GetExperienceFromKilling.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Systems/Helpers/SpawnEffect.h"
#include "Systems/Helpers/GetPrimaryMaterial.h"

using namespace entt::literals;

void drft::system::HealthSystem::init()
{
	_dispatcher->sink<events::TurnStartEvent>().connect<&HealthSystem::onTurnStartEvent>(this);
	_registry->on_update<component::action::LevelUp>().connect<&HealthSystem::onLevelUp>(this);
	_registry->on_construct<HealthComponent>().connect<&HealthSystem::onHealthComponentAdded>(this);
}

void drft::system::HealthSystem::update(const float dt)
{
	// This sepration of incoming / taking damage allows for event handlers to react to the events separately
	auto incomingDamageView = _registry->view<component::action::IncomingDamage>();
	for (auto [entity, incoming] : incomingDamageView.each())
	{
		int total = 0;
		for (auto& [_, damage] : incoming.damageTypes)
		{
			total += damage;
		}
		_registry->emplace<component::action::TakeDamage>(entity, total, incoming.source);
	}

	auto damageView = _registry->view<component::action::TakeDamage, HealthComponent>();
	for (auto [entity, damage, health] : damageView.each())
	{
		auto handle = entt::handle{ *_registry, entity };
		// send floating message
		if (auto posComp = handle.try_get<PositionComponent>())
		{
			sf::Color materialColor = sf::Color::White;
			auto optionalMaterial = getPrimaryMaterial(handle);
			if (optionalMaterial.has_value())
			{
				const RenderComponent& materialRender = optionalMaterial.value().get<RenderComponent>();
				materialColor = materialRender.color;
			}

			std::string message;
			const auto& renderComponent = handle.get<RenderComponent>();
			sf::Color messageColor = sf::Color::White;
			
			RenderComponent effectRender = renderComponent;
			int effect_ttl = 10;

			if (damage.amount == 0)
			{
				messageColor = sf::Color::Blue;
				effectRender.color = sf::Color(180, 180, 180);
				effectRender.texture = "simpleTileset"_hs;
				effectRender.uvCoords = { 0, 5 };
				effect_ttl = 30;
			}
			else if (damage.amount < 0)
			{
				message += "+";
				messageColor = sf::Color::Green;
				effectRender.color = sf::Color::Green;
			}
			else if (damage.amount > 0)
			{
				std::vector<RenderComponent> hitParticles =
				{
					RenderComponent{.texture = "simpleTileset"_hs, .uvSize = {16, 16}, .uvCoords{0, 8}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = materialColor},
					RenderComponent{.texture = "simpleTileset"_hs, .uvSize = {16, 16}, .uvCoords{1, 8}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = materialColor},
					RenderComponent{.texture = "simpleTileset"_hs, .uvSize = {16, 16}, .uvCoords{2, 8}, .layer = static_cast<unsigned int>(RenderLayer::EffectsBack), .color = materialColor},
				};
				// Spawn Hit particles
				spawnEffect(*_registry, {
					.sprites = std::move(hitParticles),
					.position = posComp->position,
					.animationSpeed = 12.0f
					});
			}

			_dispatcher->trigger(events::SendFloatingMessageEvent{
				.message = message + std::to_string(std::abs(damage.amount)),
				.color = messageColor,
				.position = posComp->position,
				.velocity = {0,-1},
				.fades = true,
				.isScreenSpace = false,
				.ttl = 80
				});

			// Spawn HurtEffect
			spawnEffect(*_registry, {
				.sprites = { effectRender },
				.position = posComp->position,
				.animationSpeed = 10.0f,
				.ttl = effect_ttl,
				.fades = false,
			});
		}
		
		health.current = std::clamp(health.current - damage.amount, 0.f, health.max);
		if (health.current == 0)
		{
			handle.emplace<component::action::Die>();
			_registry->emplace_or_replace<component::action::GainExperience>(damage.source, getExperienceFromKilling(handle));
		}
	}
}

void drft::system::HealthSystem::onUpdateEnd()
{
	_registry->clear<component::action::IncomingDamage>();
	_registry->clear<component::action::TakeDamage>();
}

void drft::system::HealthSystem::onTurnStartEvent(events::TurnStartEvent& ev)
{
	if (auto health = _registry->try_get<HealthComponent>(ev.entity))
	{
		health->current = std::clamp(health->current + health->recovery, 1.f, health->max);
	}
}

void drft::system::HealthSystem::onHealthComponentAdded(entt::registry& registry, entt::entity entity)
{
	auto& healthComponent = registry.get<HealthComponent>(entity);
	if (healthComponent.current == std::numeric_limits<float>::min())
	{
		healthComponent.current = healthComponent.max;
	}
}

void drft::system::HealthSystem::onLevelUp(entt::registry& registry, entt::entity entity)
{
	if (auto health = registry.try_get<HealthComponent>(entity))
	{
		const auto& levelUp = registry.get<component::action::LevelUp>(entity);
		if (levelUp.statChanges.contains("vitality"))
		{
			health->max += levelUp.statChanges.at("vitality");
		}
		health->current = health->max;
	}
}
