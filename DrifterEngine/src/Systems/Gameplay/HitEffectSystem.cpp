#include "pch.h"
#include "HitEffectSystem.h"
#include "Components/Components.h"
#include "Utility/SpriteIndexer.h"
#include "Spatial/Conversions.h"

constexpr float ALPHA_DROP_RATE = 30;

void drft::system::HitEffectSystem::init()
{
	registry->on_construct<component::action::TakeDamage>().connect<&HitEffectSystem::onTakeDamage>(this);
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
		queueHitEffect(pos->position);
	}
}

void drft::system::HitEffectSystem::queueHitEffect(sf::Vector2f position)
{
	auto effect = entt::handle{ *registry, registry->create() };
	effect.emplace<component::Render>(static_cast<unsigned int>(util::Sprite::Square), sf::Color::White);
	effect.emplace<component::Position>(position, static_cast<int>(spatial::Layer::Effect));

	_hitEffects.push_back(effect);
}
