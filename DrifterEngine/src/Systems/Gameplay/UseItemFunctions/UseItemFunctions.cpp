#include "pch.h"
#include "UseItemFunctions.h"
#include "Components/Components.h"

void drft::system::use::heal(entt::registry& registry, entt::entity user, Parameters params)
{
	int amount = static_cast<int>(params.at("Amount"));
	registry.emplace_or_replace<component::action::TakeDamage>(user, -amount);
}
