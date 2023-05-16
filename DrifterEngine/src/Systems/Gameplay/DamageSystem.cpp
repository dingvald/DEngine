#include <pch.h>
#include "DamageSystem.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"

void drft::system::DamageSystem::init()
{
}

void drft::system::DamageSystem::update(const float dt)
{
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
		
		handle.remove<component::action::TakeDamage>();
	}
}
