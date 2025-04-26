#include "pch.h"
#include "SpriteControllerSystem.h"
#include "Components/SpriteControllerComponent.h"
#include "Components/SpriteChangeRequestComponent.h"
#include "Components/RenderComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/SyncedAnimationComponent.h"

#include <Utility/StandardLogger.h>
#include <Utility/EntityAccessors/GetEntityName.h>

using namespace entt::literals;

void drft::system::SpriteControllerSystem::init()
{
	_registry.on_construct<SpriteControllerComponent>().connect<&SpriteControllerSystem::onSpriteControllerAdded>(this);
	_registry.on_construct<SpriteChangeRequestComponent>().connect<&SpriteControllerSystem::onSpriteChangeRequest>(this);
}

/*static*/RenderComponent drft::system::SpriteControllerSystem::getDefaultRenderComponent(entt::const_handle entity)
{
	RenderComponent result = DebugRenderComponent;
	if (auto spriteController = entity.try_get<SpriteControllerComponent>())
	{
		auto defaultVariant = spriteController->states.at("default"_hs);
		while (auto otherID = std::get_if<entt::id_type>(&defaultVariant))
		{
			defaultVariant = spriteController->states.at(*otherID);
		}

		if (auto node = std::get_if<SpriteControllerComponent::StateNode>(&defaultVariant))
		{
			applySpriteOptionsToRenderComponent(result, node->frames[0]);
			return result;
		}
	}

	if (auto render = entity.try_get<RenderComponent>())
	{
		result = *render;
	}

	return result;
}

void drft::system::SpriteControllerSystem::onSpriteControllerAdded(entt::registry& registry, entt::entity entity) const
{
	auto& controller = registry.get<SpriteControllerComponent>(entity);
	if (!controller.states.contains("default"_hs))
	{
		error_logger << "Error: " << util::getEntityName({ registry, entity }) << "'s SpriteContollerComponent default state missing." << std::endl;
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
	_registry.clear<SpriteChangeRequestComponent>();
}

void drft::system::SpriteControllerSystem::handleNewSpriteState(entt::handle handle, SpriteControllerComponent& controller, entt::id_type stateId) const
{
	const auto& nodeVariant = controller.states.at(stateId);
	if (auto otherNode = std::get_if<entt::id_type>(&nodeVariant))
	{
		if (!controller.states.contains(*otherNode))
		{
			error_logger << "Error: " << util::getEntityName(handle) << "'s SpriteControllerComponent is missing the referenced ID" << std::endl;
			return;
		}

		if (auto id = std::get_if<entt::id_type>(&controller.states.at(*otherNode)); *id == stateId)
		{
			error_logger << "Error: " << util::getEntityName(handle) << "'s SpriteControllerComponent contains a circular reference in its states." << std::endl;
			return;
		}

		handleNewSpriteState(handle, controller, *otherNode);
	}
	else if (auto node = std::get_if<SpriteControllerComponent::StateNode>(&nodeVariant))
	{
		if (node->frames.empty())
		{
			error_logger << "Error: " << util::getEntityName(handle) << "'s SpriteControllerComponent contains no states." << std::endl;
			return;
		}

		if (node->synced)
		{
			handle.emplace_or_replace<SyncedAnimationComponent>(node->frames, node->speed.value_or(1.0f), true);
		}
		else
		{
			handle.emplace_or_replace<AnimationComponent>(node->frames, node->speed.value_or(1.0f), true);
		}

		joinWithRenderComponent(handle, node->frames[0]);
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


