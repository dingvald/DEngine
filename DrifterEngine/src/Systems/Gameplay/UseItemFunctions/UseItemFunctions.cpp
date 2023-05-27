#include "pch.h"
#include "UseItemFunctions.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"

void drft::system::use::heal(entt::registry& registry, entt::entity user, Parameters params)
{
	int amount = std::stoi(params.at("Amount"));
	registry.emplace_or_replace<component::action::TakeDamage>(user, -amount);
}

void drft::system::use::read(entt::registry& registry, entt::entity user, Parameters params)
{
	std::cout << "Reading the item..." << std::endl;
	registry.emplace_or_replace<component::action::DisplayText>(user, params);
	registry.ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::DisplayText });
}
