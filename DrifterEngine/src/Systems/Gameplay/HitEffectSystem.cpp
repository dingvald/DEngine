#include "pch.h"
#include "HitEffectSystem.h"
#include "Components/Components.h"
#include "Utility/SpriteIndexer.h"
#include "Spatial/Conversions.h"

constexpr float ALPHA_DROP_RATE = 30;

void drft::system::HitEffectSystem::init()
{
	registry->on_construct<component::action::TakeDamage>().connect<&HitEffectSystem::onTakeDamage>(this);
	registry->on_construct<component::action::Die>().connect<&HitEffectSystem::onDie>(this);
}

void drft::system::HitEffectSystem::fixedUpdate()
{
	if (_hitEffects.empty()) return;

	std::vector<entt::entity> toRemove;
	for (auto effect : _hitEffects)
	{
		auto& render = registry->get<component::Render>(effect);
		float alpha = static_cast<float>(render.color.a);
		alpha -= ALPHA_DROP_RATE;
		if (alpha <= 0)
		{
			registry->destroy(effect);
			toRemove.push_back(effect);
		}
		else
		{
			render.color.a = static_cast<sf::Uint8>(alpha);
		}
	}
	for (auto effect : toRemove)
	{
		_hitEffects.erase(std::remove(_hitEffects.begin(), _hitEffects.end(), effect), _hitEffects.end());
	}
}

void drft::system::HitEffectSystem::onTakeDamage(entt::registry& registry, entt::entity entity)
{
	if (const auto& pos = registry.try_get<component::Position>(entity))
	{
		const auto& damage = registry.get<component::action::TakeDamage>(entity);
		if (damage.amount < 0)
		{
			queueHitEffect(pos->position, sf::Color::Green);
		}
		else
		{
			queueHitEffect(pos->position, sf::Color::White);
		}
	}
}

void drft::system::HitEffectSystem::onDie(entt::registry& registry, entt::entity entity)
{
	if (const auto& pos = registry.try_get<component::Position>(entity))
	{
		queueHitEffect(pos->position, sf::Color::Red);
	}
}

void drft::system::HitEffectSystem::queueHitEffect(sf::Vector2f position, sf::Color color)
{
	auto effect = entt::handle{ *registry, registry->create() };
	effect.emplace<component::Render>(static_cast<unsigned int>(util::Sprite::Square), 4u, color);
	effect.emplace<component::Position>(position);
	_hitEffects.push_back(effect);
}
