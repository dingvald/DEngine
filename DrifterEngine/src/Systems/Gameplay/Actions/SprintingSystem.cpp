#include "pch.h"
#include "SprintingSystem.h"
#include "Components/Components.h"
#include "Utility/SpriteIndexer.h"

static constexpr float PI = 3.141592f;

void drft::system::SprintingSystem::init()
{
	registry->on_construct<component::Sprinting>().connect<&SprintingSystem::onSprintingAdded>(this);
	registry->on_destroy<component::Sprinting>().connect<&SprintingSystem::onSprintingRemoved>(this);
}

void drft::system::SprintingSystem::fixedUpdate()
{
	auto sprintView = registry->view<component::Sprinting, component::Stamina, const component::Position>();
	for (auto [entity, sprinting, stamina, pos] : sprintView.each())
	{
		if (stamina.current <= 0.f)
		{
			registry->remove<component::Sprinting>(entity);
			continue;
		}
		if (!_sprintEffects.contains(entity))
		{
			addSprintEffect(*registry, entity);
		}
		registry->patch<component::Position>(_sprintEffects[entity],
			[pos](component::Position& position)
			{
				position.position = pos.position;
			});
	}

	animateSprintEffects();
}

void drft::system::SprintingSystem::onFixedUpdateEnd()
{

}

void drft::system::SprintingSystem::shutdown()
{
	for (auto&& [sprinter, effect] : _sprintEffects)
	{
		registry->destroy(effect);
	}
}

void drft::system::SprintingSystem::onSprintingAdded(entt::registry& registry, entt::entity entity)
{
	applySprintBuff(registry, entity);
	addSprintEffect(registry, entity);
}

void drft::system::SprintingSystem::onSprintingRemoved(entt::registry& registry, entt::entity entity)
{
	removeSprintBuff(registry, entity);
	removeSprintEffect(registry, entity);
}

void drft::system::SprintingSystem::applySprintBuff(entt::registry& registry, entt::entity entity)
{
	if (auto actor = registry.try_get<component::Actor>(entity))
	{
		actor->moveSpeed += 2.f;
	}
	if (auto stamina = registry.try_get<component::Stamina>(entity))
	{
		stamina->baseConsumption += 1.f;
	}
}

void drft::system::SprintingSystem::removeSprintBuff(entt::registry& registry, entt::entity entity)
{
	if (auto actor = registry.try_get<component::Actor>(entity))
	{
		actor->moveSpeed -= 2.f;
	}
	if (auto stamina = registry.try_get<component::Stamina>(entity))
	{
		stamina->baseConsumption -= 1.f;
	}
}

void drft::system::SprintingSystem::addSprintEffect(entt::registry& registry, entt::entity entity)
{
	if (_sprintEffects.contains(entity)) return;
	auto effect = entt::handle{ registry, registry.create() };
	effect.emplace<component::Render>(static_cast<unsigned int>(util::Sprite::StatusEffect), 4u, sf::Color(50,150,50));
	effect.emplace<component::Position>(sf::Vector2i(0,0));
	_sprintEffects.emplace(entity, effect.entity());
}

void drft::system::SprintingSystem::removeSprintEffect(entt::registry& registry, entt::entity entity)
{
	registry.destroy(_sprintEffects.at(entity));
	_sprintEffects.erase(entity);
}

void drft::system::SprintingSystem::animateSprintEffects() const
{
	static int frames = 0;
	if (frames >= 360) frames = 0;
	for (auto [sprinter, effect] : _sprintEffects)
	{
		auto& render = registry->get<component::Render>(effect);
		float alpha = 255 * ((std::sinf(frames * (PI / 180.f)) + 1.f) / 2.f);
		render.color.a = static_cast<sf::Uint8>(alpha);
	}
	frames+=5;
}


