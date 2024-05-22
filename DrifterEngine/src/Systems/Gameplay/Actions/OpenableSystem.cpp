#include "pch.h"
#include "OpenableSystem.h"
#include "Components/Components.h"
#include "Systems/Helpers/ContainerHasItem.h"
#include "Events/SendFloatingMessageEvent.h"

void drft::system::OpenableSystem::init()
{
	_registry->on_construct<component::Openable>().connect<&OpenableSystem::onConstructOpenable>(this);
	_registry->on_destroy<component::Openable>().connect<&OpenableSystem::onDestroyOpenable>(this);
}

void drft::system::OpenableSystem::openInteraction(entt::entity actor, entt::entity subject) const
{
	if (auto openable = _registry->try_get<component::Openable>(subject))
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
			bool isContainer = _registry->any_of<component::Container>(subject);
			auto& render = _registry->get<component::Render>(subject);
			render.sprite += 1;
			render.layer = 1;
			auto& physical = _registry->get<component::Physical>(subject);
			physical.blocks = false;
			if (!isContainer)
			{
				_registry->remove<component::LightBlocking>(subject);
			}
			openable->isOpen = true;
			toggleInteractionFunction(subject);
		}
		else
		{
			auto& dispatcher = _registry->ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::SendFloatingMessageEvent{
				.message = "Key required.",
				.color = sf::Color::Red,
				.position = _registry->get<component::Position>(actor).position,
				.velocity = {0,0},
				.isScreenSpace = false,
				.ttl = 120
				});
		}
	}
}

void drft::system::OpenableSystem::closeInteraction(entt::entity actor, entt::entity subject) const
{
	if (auto openable = _registry->try_get<component::Openable>(subject))
	{
		auto& render = _registry->get<component::Render>(subject);
		render.sprite -= 1;
		render.layer = 2;

		auto& physical = _registry->get<component::Physical>(subject);
		physical.blocks = true;

		bool isContainer = _registry->any_of<component::Container>(subject);
		if (!isContainer)
		{
			_registry->emplace_or_replace<component::LightBlocking>(subject);
		}
		openable->isOpen = false;
		toggleInteractionFunction(subject);
	}
}

void drft::system::OpenableSystem::toggleInteractionFunction(entt::entity openable) const
{
	if (auto interactable = _registry->try_get<component::Interactable>(openable))
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
	auto& openable = registry.get<component::Openable>(entity);
	auto& interactable = registry.get_or_emplace<component::Interactable>(entity);
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
	if (auto interactable = registry.try_get<component::Interactable>(entity))
	{
		interactable->interactions.erase("Open");
		interactable->interactions.erase("Close");
	}
}
