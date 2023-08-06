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
	if (_effects.empty()) return;

	std::vector<entt::entity> toRemove;
	for (auto effect : _effects)
	{
		if (!_effectUpdateFunctions.at(effect)(*registry, effect))
		{
			toRemove.push_back(effect);
		}
	}
	for (auto effect : toRemove)
	{
		removeEffect(effect);
	}
}

void drft::system::HitEffectSystem::onTakeDamage(entt::registry& registry, entt::entity entity)
{
	if (const auto& pos = registry.try_get<component::Position>(entity))
	{
		
		const auto& damage = registry.get<component::action::TakeDamage>(entity);

		auto fadeFunc = [](entt::registry& registry, entt::entity effect) -> bool
		{
			auto& render = registry.get<component::Render>(effect);
			float alpha = static_cast<float>(render.color.a);
			alpha -= ALPHA_DROP_RATE;
			if (alpha <= 0)
			{
				return false;
			}
			else
			{
				render.color.a = static_cast<sf::Uint8>(alpha);
			}
			return true;
		};
		auto halfFadeFunc = [](entt::registry& registry, entt::entity effect) -> bool
		{
			auto& render = registry.get<component::Render>(effect);
			float alpha = static_cast<float>(render.color.a);
			alpha -= (ALPHA_DROP_RATE/2);
			if (alpha <= 0)
			{
				return false;
			}
			else
			{
				render.color.a = static_cast<sf::Uint8>(alpha);
			}
			return true;
		};
		auto quarterFadeFunc = [](entt::registry& registry, entt::entity effect) -> bool
		{
			auto& render = registry.get<component::Render>(effect);
			float alpha = static_cast<float>(render.color.a);
			alpha -= (ALPHA_DROP_RATE / 4);
			if (alpha <= 0)
			{
				return false;
			}
			else
			{
				render.color.a = static_cast<sf::Uint8>(alpha);
			}
			return true;
		};

		if (damage.amount < 0)
		{
			queueEffect(pos->position, static_cast<unsigned int>(util::Sprite::Square), sf::Color::Green, quarterFadeFunc);
				
		}
		else if (damage.amount > 0)
		{
			unsigned int sprite = static_cast<unsigned int>(util::Sprite::Square);
			queueEffect(pos->position, sprite, sf::Color::White, fadeFunc);
		}

		if (const auto incoming = registry.try_get<component::action::IncomingDamage>(entity))
		{
			if ((incoming->amount < (incoming->originalAmount / 2)) || incoming->amount == 0) // Most/all of the damage has been mitigated
			{
				unsigned int sprite = static_cast<unsigned int>(util::Sprite::Square);
				queueEffect(pos->position, sprite, sf::Color::Blue, halfFadeFunc);
			}
		}
	}
}

void drft::system::HitEffectSystem::onDie(entt::registry& registry, entt::entity entity)
{
	if (const auto& pos = registry.try_get<component::Position>(entity))
	{
		auto fadeFunc = [this](entt::registry& registry, entt::entity effect) -> bool
		{
			auto& render = registry.get<component::Render>(effect);
			float alpha = static_cast<float>(render.color.a);
			alpha -= ALPHA_DROP_RATE/4;
			if (alpha <= 0)
			{
				return false;
			}
			else
			{
				render.color.a = static_cast<sf::Uint8>(alpha);
			}
			return true;
		};
		queueEffect(pos->position, static_cast<unsigned int>(util::Sprite::Square), sf::Color::Red, fadeFunc);
	}
}

void drft::system::HitEffectSystem::queueEffect(sf::Vector2i position, unsigned int sprite, sf::Color color, std::function<bool(entt::registry& registry, entt::entity)> effectUpdateFunc)
{
	auto effect = entt::handle{ *registry, registry->create() };
	effect.emplace<component::Render>(sprite, 4u, color);
	effect.emplace<component::Position>(position);
	_effects.emplace_back(effect.entity());
	_effectUpdateFunctions.emplace(effect.entity(), effectUpdateFunc);
}

void drft::system::HitEffectSystem::removeEffect(entt::entity effect)
{
	registry->destroy(effect);
	_effects.erase(std::remove(_effects.begin(), _effects.end(), effect), _effects.end());
	_effectUpdateFunctions.erase(effect);
}
