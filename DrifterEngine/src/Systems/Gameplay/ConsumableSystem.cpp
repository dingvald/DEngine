#include "pch.h"
#include "ConsumableSystem.h"
#include "Components/Components.h"
#include "Components/ConsumableComponent.h"
#include "Components/InteractableComponent.h"

void drft::system::ConsumableSystem::init()
{
	_registry.on_construct<ConsumableComponent>().connect<&ConsumableSystem::onConstructConsumable>(this);
	_registry.on_destroy<ConsumableComponent>().connect<&ConsumableSystem::onDestroyConsumable>(this);
}

void drft::system::ConsumableSystem::consumeInteraction(entt::entity actor, entt::entity subject) const
{
	if (auto consumable = _registry.try_get<ConsumableComponent>(subject))
	{
		_registry.emplace<component::action::ConsumeEntity>(actor, subject);
		_registry.erase<component::action::ConsumeEntity>(actor);
		if (consumable->destroy)
		{
			_registry.destroy(subject);
		}
	}
}

void drft::system::ConsumableSystem::onConstructConsumable(entt::registry& registry, entt::entity entity) const
{
	auto& interactable = registry.get_or_emplace<InteractableComponent>(entity);
	interactable.interactions["Consume"] = [this](auto e1, auto e2) {this->consumeInteraction(e1, e2); };
}

void drft::system::ConsumableSystem::onDestroyConsumable(entt::registry& registry, entt::entity entity) const
{
	if (auto interactable = registry.try_get<InteractableComponent>(entity))
	{
		interactable->interactions.erase("Consume");
	}
}