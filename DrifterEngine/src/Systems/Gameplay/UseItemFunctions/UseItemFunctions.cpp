#include "pch.h"
#include "UseItemFunctions.h"
#include "Components/Components.h"
#include "Systems/Helpers/ContainerHasItem.h"
#include "Events/RequestStateChange.h"
#include "Events/SendFloatingMessageEvent.h"

void drft::system::use::heal(entt::registry& registry, entt::entity user, entt::entity item, Parameters params)
{
	int amount = std::stoi(params.at("Amount"));
	registry.emplace_or_replace<component::action::TakeDamage>(user, -amount);
}

void drft::system::use::read(entt::registry& registry, entt::entity user, entt::entity item, Parameters params)
{
	std::cout << "Reading the item..." << std::endl;
	registry.emplace_or_replace<component::action::DisplayText>(user, params);
	registry.ctx().get<entt::dispatcher&>().trigger(events::RequestStateStackPush{ States::DisplayText });
}

void drft::system::use::open(entt::registry& registry, entt::entity user, entt::entity item, Parameters params)
{
	if (auto openable = registry.try_get<component::Openable>(item))
	{
		bool canOpen = true;
		if (openable->keyName == "")
		{
			// check if you have the key...
			canOpen = containerHasItem(registry, user, openable->keyName);
			//TODO: Consume key
		}

		if (canOpen)
		{
			// chest if container - door otherwise
			bool isContainer = registry.any_of<component::Container>(item);
			if (openable->isOpen)
			{
				auto& render = registry.get<component::Render>(item);
				render.sprite -= 1;
				render.layer = 2;

				auto& physical = registry.get<component::Physical>(item);
				physical.blocks = true;
				if (!isContainer)
				{
					registry.emplace_or_replace<component::LightBlocking>(item);
				}
				openable->isOpen = false;
			}
			else
			{
				auto& render = registry.get<component::Render>(item);
				render.sprite += 1;
				render.layer = 1;
				auto& physical = registry.get<component::Physical>(item);
				physical.blocks = false;
				if (!isContainer)
				{
					registry.remove<component::LightBlocking>(item);
				}
				openable->isOpen = true;
			}
		}
		else
		{
			auto& dispatcher = registry.ctx().get<entt::dispatcher&>();
			dispatcher.trigger(events::SendFloatingMessageEvent{
				.message = "Cannot open - Key Required.",
				.color = sf::Color::White,
				.position = registry.get<component::Position>(user).position,
				.velocity = {0,0},
				.isScreenSpace = false,
				.ttl = 240
				});
		}
	}
}
