#include "pch.h"
#include "StaminaSystem.h"
#include "Components/Components.h"

void drft::system::StaminaSystem::init()
{

}

void drft::system::StaminaSystem::update(const float dt)
{
	auto spendStaminaView = registry->view<component::Stamina, component::action::ConsumeStamina>();
	for (auto [entity, stamina, staminaSpend] : spendStaminaView.each())
	{
		stamina.current = std::clamp(stamina.current - (stamina.baseConsumption + staminaSpend.amount), 0, stamina.max);
	}
}

void drft::system::StaminaSystem::onUpdateEnd()
{
	registry->clear<component::action::ConsumeStamina>();
}
