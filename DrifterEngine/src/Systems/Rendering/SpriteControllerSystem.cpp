#include "pch.h"
#include "SpriteControllerSystem.h"
#include "Components/SpriteControllerComponent.h"
#include "Components/SpriteChangeRequestComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/SyncedAnimationComponent.h"

using namespace entt::literals;

void drft::system::SpriteControllerSystem::init()
{
	_registry.on_construct<SpriteControllerComponent>().connect<&SpriteControllerSystem::onSpriteControllerAdded>(this);
	_registry.on_construct<SpriteChangeRequestComponent>().connect<&SpriteControllerSystem::onSpriteChangeRequest>(this);
}

void drft::system::SpriteControllerSystem::onUpdateEnd()
{
	_registry.clear<SpriteChangeRequestComponent>();
}

void drft::system::SpriteControllerSystem::onSpriteControllerAdded(entt::registry& registry, entt::entity entity) const
{
	auto& controller = registry.get<SpriteControllerComponent>(entity);
	if (!controller.states.contains("default"_hs))
	{
		throw std::exception("The SpriteController requires a default state");
		return;
	}

	handleNewSpriteState({ registry, entity }, controller, "default"_hs);
}

void drft::system::SpriteControllerSystem::onSpriteChangeRequest(entt::registry& registry, entt::entity entity) const
{
	if (auto controller = registry.try_get<SpriteControllerComponent>(entity))
	{
		auto& request = registry.get<SpriteChangeRequestComponent>(entity);
		if (!controller->states.contains(request.stateId)) return;

		handleNewSpriteState({ registry, entity }, *controller, request.stateId);
	}
}

void drft::system::SpriteControllerSystem::handleNewSpriteState(entt::handle handle, SpriteControllerComponent& controller, entt::id_type stateId) const
{
	const auto& nodeVariant = controller.states.at(stateId);
	if (auto otherNode = std::get_if<entt::id_type>(&nodeVariant))
	{
		// TODO: Maybe don't fail silently here..?
		if (!controller.states.contains(*otherNode)) return;
		if (std::get_if<entt::id_type>(&controller.states.at(*otherNode)) != nullptr) return;

		handleNewSpriteState(handle, controller, *otherNode);
	}
	else if (auto node = std::get_if<SpriteControllerComponent::StateNode>(&nodeVariant))
	{
		if (node->frames.empty()) return;

		if (node->frames.size() == 1)
		{
			auto& frame = node->frames.front();
			joinWithRenderComponent(handle, frame);
		}
		else if (!node->synced)
		{
			AnimationComponent animation;
			for (auto&& frame : node->frames)
			{
				animation.frames.emplace_back(frame);
			}
			animation.speed = node->speed.value_or(1.0f);
			animation.loops = true;
			handle.emplace_or_replace<AnimationComponent>(std::move(animation));
		}
		else if (node->synced)
		{
			SyncedAnimationComponent animation;
			for (auto&& frame : node->frames)
			{
				animation.frames.emplace_back(frame);
			}
			animation.speed = node->speed.value_or(1.0f);
			animation.loops = true;
			handle.emplace_or_replace<SyncedAnimationComponent>(std::move(animation));
		}
	}
}

void drft::system::SpriteControllerSystem::joinWithRenderComponent(entt::handle entity, const SpriteOptions& options) const
{
	if (auto render = entity.try_get<RenderComponent>())
	{
		applySpriteOptionsToRenderComponent(*render, options);
	}
	else
	{
		RenderComponent newRender;
		applySpriteOptionsToRenderComponent(newRender, options);
		entity.emplace<RenderComponent>(std::move(newRender));
	}
}


