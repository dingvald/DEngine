#include "pch.h"
#include "OpenableSystem.h"

#include "Components/ContainerComponent.h"
#include "Components/OpenableComponent.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/LightBlockingComponent.h"
#include "Components/InteractableComponent.h"

#include "Systems/Helpers/ContainerHasItem.h"
#include "Events/SendFloatingMessageEvent.h"

void drft::system::OpenableSystem::init()
{
	_registry->on_construct<OpenableComponent>().connect<&OpenableSystem::onConstructOpenable>(this);
	_registry->on_destroy<OpenableComponent>().connect<&OpenableSystem::onDestroyOpenable>(this);
}

void drft::system::OpenableSystem::openInteraction(entt::entity actor, entt::entity subject) const
{
	if (auto openable = _registry->try_get<OpenableComponent>(subject))
	{
		bool canOpen = true;
		if (openable->keyName != "")
		{
			canOpen = containerHasItem(*_registry, actor, openable->keyName);
			//TODO: Consume key
		}

		if (canOpen)
		{
			// chest if container - door otherwise
			bool isContainer = _registry->any_of<ContainerComponent>(subject);
			auto& render = _registry->get<RenderComponent>(subject);
			render.uvCoords += {1, 0};
			render.layer = 1;
			auto& material = _registry->get<MaterialComponent>(subject);
			material.blocks = false;
			if (!isContainer)
			{
				_registry->remove<LightBlockingComponent>(subject);
			}
			openable->isOpen = true;
			toggleInteractionFunction(subject);
		}
		else
		{
			_dispatcher->trigger(events::SendFloatingMessageEvent{
				.message = "Key required.",
				.color = sf::Color::Red,
				.position = _registry->get<PositionComponent>(actor).position,
				.velocity = {0,0},
				.isScreenSpace = false,
				.ttl = 120
				});
		}
	}
}

void drft::system::OpenableSystem::closeInteraction(entt::entity actor, entt::entity subject) const
{
	if (auto openable = _registry->try_get<OpenableComponent>(subject))
	{
		auto& render = _registry->get<RenderComponent>(subject);
		render.uvCoords -= {1, 0};
		render.layer = 2;

		auto& material = _registry->get<MaterialComponent>(subject);
		material.blocks = true;

		bool isContainer = _registry->any_of<ContainerComponent>(subject);
		if (!isContainer)
		{
			_registry->emplace_or_replace<LightBlockingComponent>(subject);
		}
		openable->isOpen = false;
		toggleInteractionFunction(subject);
	}
}

void drft::system::OpenableSystem::toggleInteractionFunction(entt::entity openable) const
{
	if (auto interactable = _registry->try_get<InteractableComponent>(openable))
	{
		if (interactable->interactions.contains("Open"))
		{
			interactable->interactions["Close"] = [this](auto e1, auto e2) { this->closeInteraction(e1, e2); };
			interactable->interactions.erase("Open");
		}
		else if (interactable->interactions.contains("Close"))
		{
			interactable->interactions["Open"] = [this](auto e1, auto e2) { this->openInteraction(e1, e2); };
			interactable->interactions.erase("Close");
		}
	}
}

void drft::system::OpenableSystem::onConstructOpenable(entt::registry& registry, entt::entity entity) const
{
	auto& openable = registry.get<OpenableComponent>(entity);
	auto& interactable = registry.get_or_emplace<InteractableComponent>(entity);
	if (!openable.isOpen)
	{
		interactable.interactions["Open"] = [this](auto e1, auto e2) { this->openInteraction(e1, e2); };
	}
	else
	{
		interactable.interactions["Close"] = [this](auto e1, auto e2) { this->closeInteraction(e1, e2); };
	}
}

void drft::system::OpenableSystem::onDestroyOpenable(entt::registry& registry, entt::entity entity) const
{
	if (auto interactable = registry.try_get<InteractableComponent>(entity))
	{
		interactable->interactions.erase("Open");
		interactable->interactions.erase("Close");
	}
}
