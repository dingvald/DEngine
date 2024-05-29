#include "pch.h"
#include "StaminaSystem.h"
#include "Components/Components.h"
#include "Components/StaminaComponent.h"

void drft::system::StaminaSystem::init()
{
	_registry->on_construct<StaminaComponent>().connect<&StaminaSystem::onStaminaAdded>(this);
}

void drft::system::StaminaSystem::update(const float dt)
{
	auto spendStaminaView = _registry->view<StaminaComponent, component::action::ConsumeStamina>();
	for (auto [entity, stamina, staminaSpend] : spendStaminaView.each())
	{
		stamina.current = std::clamp(stamina.current - (stamina.baseConsumption + staminaSpend.amount), 0.f, stamina.max);
	}
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
