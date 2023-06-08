#include <pch.h>
#include "DamageSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"

void drft::system::DamageSystem::init()
{
}

void drft::system::DamageSystem::update(const float dt)
{
	// This sepration of incoming / taking damage allows for event handlers to react to the events separately
	auto incomingDamageView = registry->view<component::action::IncomingDamage>();
	for (auto [entity, incoming] : incomingDamageView.each())
	{
		registry->emplace<component::action::TakeDamage>(entity, incoming.amount);
	}

	auto damageView = registry->view<component::action::TakeDamage>();
	for (auto [entity, damage] : damageView.each())
	{
		entt::handle handle = { *registry, entity };
		auto health = handle.try_get<component::Health>();
		if (!health) continue;
		
		health->current -= damage.amount;
		std::cout << "The " << util::getEntityName(handle) << " takes " << damage.amount << " damage!" << std::endl;

		health->current = std::clamp(health->current, 0, health->max);

		if (health->current == 0)
		{
			handle.emplace<component::action::Die>();
		}
	}
}

void drft::system::DamageSystem::onUpdateEnd()
{
	registry->clear<component::action::IncomingDamage>();
	registry->clear<component::action::TakeDamage>();
}
