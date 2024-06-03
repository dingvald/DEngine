#include "pch.h"
#include "StaminaSystem.h"
#include "Components/Components.h"
#include "Components/Actions/MoveAction.h"
#include "Components/StaminaComponent.h"

void drft::system::StaminaSystem::init()
{
	_registry->on_construct<StaminaComponent>().connect<&StaminaSystem::onStaminaAdded>(this);
	_registry->on_construct<component::action::ConsumeStamina>().connect<&StaminaSystem::onStaminaConsumed>(this);
	_registry->on_construct<DoMoveAction>().connect<&StaminaSystem::onDoMoveAction>(this);
}

void drft::system::StaminaSystem::onUpdateEnd()
{
	_registry->clear<component::action::ConsumeStamina>();
}

void drft::system::StaminaSystem::onStaminaAdded(entt::registry& registry, entt::entity entity) const
{
	auto& staminaComponent = registry.get<StaminaComponent>(entity);
	if (staminaComponent.current == std::numeric_limits<float>::min())
	{
		staminaComponent.current = staminaComponent.max;
	}
}

void drft::system::StaminaSystem::onStaminaConsumed(entt::registry& registry, entt::entity entity) const
{
	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		auto& consumeStaminaAction = registry.get<component::action::ConsumeStamina>(entity);
		stamina->current = std::clamp(stamina->current - (stamina->baseConsumption + consumeStaminaAction.amount), 0.f, stamina->max);
	}
}

void drft::system::StaminaSystem::onDoMoveAction(entt::registry& registry, entt::entity entity) const
{
	if (auto stamina = registry.try_get<StaminaComponent>(entity))
	{
		_registry->emplace_or_replace<component::action::ConsumeStamina>(entity, -0.25f);
	}
}
