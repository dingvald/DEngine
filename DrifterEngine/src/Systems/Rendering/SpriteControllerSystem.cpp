#include "pch.h"
#include "SpriteControllerSystem.h"
#include "Components/SpriteControllerComponent.h"
#include "Components/SpriteChangeRequestComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/SprintingComponent.h"

using namespace entt::literals;

void drft::system::SpriteControllerSystem::init()
{
	_registry->on_construct<SpriteControllerComponent>().connect<&SpriteControllerSystem::onSpriteControllerAdded>(this);
	_registry->on_construct<SpriteChangeRequestComponent>().connect<&SpriteControllerSystem::onSpriteChangeRequest>(this);
}

void drft::system::SpriteControllerSystem::onUpdateEnd()
{
	_registry->clear<SpriteChangeRequestComponent>();
}

void drft::system::SpriteControllerSystem::onSpriteControllerAdded(entt::registry& registry, entt::entity entity) const
{
	auto& controller = registry.get<SpriteControllerComponent>(entity);
	if (!controller.states.contains("default"_hs))
	{
		throw std::exception("The SpriteController requires a default state");
		return;
	}

	const auto& nodes = controller.states.at("default"_hs);
	if (nodes.size() == 1)
	{
		auto& node = nodes.front();
		if (auto render = registry.try_get<RenderComponent>(entity))
		{
			render->uvCoords = node.options.uvCoords.value_or(sf::Vector2i{0,0});
			render->texture = node.options.texture.value_or(render->texture);
			render->uvSize = node.options.uvSize.value_or(render->uvSize);
			render->layer = node.options.layer.value_or(render->layer);
			render->color = node.options.color.value_or(render->color);
		}
		else
		{
			RenderComponent newRender;
			newRender.uvCoords = node.options.uvCoords.value_or(sf::Vector2i{4, 0});
			newRender.texture = node.options.texture.value_or("simple_tileset"_hs);
			newRender.uvSize = node.options.uvSize.value_or(sf::Vector2i{16, 16});
			newRender.layer = node.options.layer.value_or(2u);
			newRender.color = node.options.color.value_or(sf::Color::Magenta);
			registry.emplace<RenderComponent>(entity, newRender);
		}
	}
	else
	{
		AnimationComponent animation;
		for (auto&& node : nodes)
		{
			animation.sprites.emplace_back(node.options);
			animation.speed = node.speed.value_or(1.0f);
		}
		
		registry.emplace_or_replace<AnimationComponent>(entity, animation);
	}
}

void drft::system::SpriteControllerSystem::onSpriteChangeRequest(entt::registry& registry, entt::entity entity) const
{
	if (auto controller = registry.try_get<SpriteControllerComponent>(entity))
	{
		auto& request = registry.get<SpriteChangeRequestComponent>(entity);
		if (!controller->states.contains(request.stateId)) return;
		if (controller->states.at(request.stateId).empty()) return;

		const auto& nodes = controller->states.at(request.stateId);
		if (nodes.size() == 1)
		{
			auto& node = nodes.front();
			if (auto render = registry.try_get<RenderComponent>(entity))
			{
				render->uvCoords = node.options.uvCoords.value_or(render->uvCoords);
				render->texture = node.options.texture.value_or(render->texture);
				render->uvSize = node.options.uvSize.value_or(render->uvSize);
				render->layer = node.options.layer.value_or(render->layer);
				render->color = node.options.color.value_or(render->color);
			}
		}
		else
		{
			AnimationComponent animation;
			for (auto&& node : nodes)
			{
				animation.sprites.emplace_back(node.options);
				animation.speed = node.speed.value_or(1.0f);
			}
			registry.emplace_or_replace<AnimationComponent>(entity, animation);
		}
	}
}


