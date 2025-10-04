#include <pch.h>
#include "HealthSystem.h"

#include "Components/Components.h"
#include "Components/HealthComponent.h"
#include "Components/PositionComponent.h"
#include <Components/SkillsComponent.h>
#include "Components/RenderComponent.h"

#include "Utility/SpriteOptions.h"
#include "Systems/Helpers/GetExperienceFromKilling.h"
#include "Events/SendFloatingMessageEvent.h"
#include "Systems/Helpers/SpawnEffect.h"
#include <Utility/EntityAccessors/GetEntityMaterials.h>
#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>
#include <Random/Random.h>
#include <Systems/Gameplay/SkillsSystem.h>
#include <Skills/SkillIds.h>
#include <Utility/Math.h>
#include <Utility/Math/Logarithmic.h>

using namespace entt::literals;

namespace Internal
{
	// See Desmos graph: https://www.desmos.com/calculator/eksnemfbjo
	static const float a = 4.2f;
	static const float b = 0.1f;
	static const float c = 10.f;
	static const float d = 1.18f;
	static const float e = 4.7f;

	static float calculateMaxHealthForLevel(int level)
	{
		return drft::math::log(level, a, b, c, d, e);
	}

	static void onToughnessLevelUp(int level, entt::handle entity)
	{
		if (auto health = entity.try_get<HealthComponent>())
		{
			health->max -= calculateMaxHealthForLevel(level - 1);
			health->max += calculateMaxHealthForLevel(level);
		}
	}
}

void drft::system::HealthSystem::init()
{
	_dispatcher.sink<events::TurnStartEvent>().connect<&HealthSystem::onTurnStartEvent>(this);
	_registry.on_construct<HealthComponent>().connect<&HealthSystem::onHealthComponentAdded>(this);
	_healthAddedObserver.connect(_registry, entt::collector.group<HealthComponent, SkillsComponent>());
}

void drft::system::HealthSystem::start()
{
	SkillsSystem::registerLevelUpHandler(SkillId::Toughness, Internal::onToughnessLevelUp, _registry);
}

void drft::system::HealthSystem::update(const float dt)
{
	for (auto&& entity : _healthAddedObserver)
	{
		setupHealthComponent({ _registry, entity });
	}
	_healthAddedObserver.clear();

	auto damageView = _registry.view<component::action::TakeDamage>();
	for (auto [entity, damage] : damageView.each())
	{
		processTakeDamage(entity, damage);
	}
	_registry.erase<component::action::TakeDamage>(damageView.begin(), damageView.end());
}

void drft::system::HealthSystem::processTakeDamage(entt::entity entity, component::action::TakeDamage& damage) const
{
	auto handle = entt::handle{ _registry, entity };
	if (!handle.all_of<HealthComponent>()) return;

	spawnDamageEffects(handle, damage.amount);

	auto& health = handle.get<HealthComponent>();
	health.current = std::clamp(health.current - damage.amount, 0.f, health.max);
	if (health.current <= 0.f)
	{
		handle.emplace<component::action::Die>();
	}
}

void drft::system::HealthSystem::spawnDamageEffects(entt::const_handle entity, float damage) const
{
	auto posComp = entity.try_get<PositionComponent>();
	if (!posComp) return;

	sf::Color materialColor = sf::Color::White;
	auto optionalMaterial = util::getEntityPrimaryMaterial(entity);
	if (optionalMaterial.has_value())
	{
		const RenderComponent& materialRender = optionalMaterial.value().get<RenderComponent>();
		materialColor = materialRender.color;
	}

	std::string message;
	const auto& renderComponent = entity.get<RenderComponent>();
	sf::Color messageColor = sf::Color::White;

	SpriteOptions damageEffectSprite;
	createSpriteOptionsFromRenderComponent(damageEffectSprite, renderComponent);
	damageEffectSprite.layer = static_cast<int>(RenderLayer::EffectsBack);

	float effect_ttl = 0.05f;

	if (math::isNearZero(damage))
	{
		messageColor = sf::Color::Blue;
		damageEffectSprite.color = sf::Color::White;
		damageEffectSprite.texture = "diamond"_hs;
		effect_ttl = 0.2f;
	}
	else if (damage < 0)
	{
		message += "+";
		messageColor = sf::Color::Green;
		damageEffectSprite.color = sf::Color::Green;
		effect_ttl = 0.2f;
	}
	else if (damage > 0)
	{
		damageEffectSprite.color = sf::Color::White;
		std::vector<SpriteOptions> hitParticles =
		{
			SpriteOptions{.uvCoords = sf::Vector2i{0, 0}, .texture = "hit_particle"_hs, .layer = RenderLayer::EffectsBack, .color = materialColor},
			SpriteOptions{.uvCoords = sf::Vector2i{1, 0}, .texture = "hit_particle"_hs, .layer = RenderLayer::EffectsBack, .color = materialColor},
			SpriteOptions{.uvCoords = sf::Vector2i{2, 0}, .texture = "hit_particle"_hs, .layer = RenderLayer::EffectsBack, .color = materialColor},
		};

		sf::Vector3f randomOffset;
		rng::Random& random = _registry.ctx().get<rng::Random>();
		randomOffset.x = std::roundf(random.realInRange(-1.f, 1.f));
		randomOffset.y = std::roundf(random.realInRange(-1.f, 1.f));

		// Spawn Hit particles
		spawnEffect(_registry, {
			.frames = std::move(hitParticles),
			.position = posComp->tile,
			.offset = randomOffset,
			.animationSpeed = 8.f,
			.requiresInFOV = false
			});
	}

	_dispatcher.trigger(events::SendFloatingMessageEvent{
		.message = message + std::format("{:.0f}", std::abs(damage)),
		.color = messageColor,
		.position = spatial::toXY(spatial::toFloatSpace(posComp->tile)),
		.velocity = {0.f,-1.f},
		.fades = true,
		.isScreenSpace = false,
		.ttl = 80
		});

	// Spawn Hurt Effect
	spawnEffect(_registry, {
		.frames = { damageEffectSprite },
		.position = posComp->tile,
		.ttl = effect_ttl,
		.fadeFunc = [](float f) {return 1.0f;},
		.requiresInFOV = false
		});
}

void drft::system::HealthSystem::onTurnStartEvent(events::TurnStartEvent& ev)
{
	if (auto health = _registry.try_get<HealthComponent>(ev.entity))
	{
		health->current = std::clamp(health->current + health->recovery, 1.f, health->max);
	}
}

void drft::system::HealthSystem::onHealthComponentAdded(entt::registry& registry, entt::entity entity) const
{
	auto& healthComponent = registry.get<HealthComponent>(entity);
	if (healthComponent.current == std::numeric_limits<float>::min())
	{
		healthComponent.current = healthComponent.max;
	}
}

void drft::system::HealthSystem::setupHealthComponent(entt::handle entity) const
{
	auto healthComponent = entity.try_get<HealthComponent>();
	if (!healthComponent) return;

	if (healthComponent->max == std::numeric_limits<float>::min())
	{
		int toughness = SkillsSystem::getSkillLevel(SkillId::Toughness, entity);
		healthComponent->max = Internal::calculateMaxHealthForLevel(toughness);
		healthComponent->current = healthComponent->max;
	}
}
