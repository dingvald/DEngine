#include "pch.h"
#include "HotbarSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Ability/AbilityRegistry.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Systems/Helpers/ToHotbarIndex.h"

void drft::system::HotbarSystem::init()
{
	AbilityRegistry::bind();
}

void drft::system::HotbarSystem::onStart(bool isNewGame)
{
	if (isNewGame)
	{
		auto playerView = registry->view<component::Player>();
		for (auto entity : playerView)
		{
			auto& hotbar = registry->emplace<component::Hotbar>(entity);
			// For testing purposes:
			hotbar.abilities[toHotbarIndex(1)] = static_cast<int>(AbilityType::Sprint);
		}
	}
}

void drft::system::HotbarSystem::update(float dt)
{
	auto view = registry->view<component::Hotbar, component::action::HotbarPressed, component::tag::CurrentActor>();
	for (auto&& [entity, hotbar, hotbarSlot] : view.each())
	{
		entt::handle handle = { *registry, entity };
		auto abilityType = static_cast<AbilityType>(hotbar.abilities[hotbarSlot.slot]);
		const auto& ability = AbilityRegistry::get(abilityType);
		if (ability.isValid(handle))
		{
			switch (ability.getTargetingType())
			{
			case AbilityTargetingType::Auto:
			{
				ability.perform(handle);
				spendActionPoints(ability.getCost(), ActionType::Act, handle);
			}
			break;
			case AbilityTargetingType::SelectDirection:
			{
				auto tilePosition = handle.get<component::Position>().position;
				registry->emplace<component::action::SelectDirection>(entity,
					[tilePosition, &ability, &handle](sf::Vector2i direction) -> bool
					{
						ability.perform(handle, tilePosition + direction);
						spendActionPoints(ability.getCost(), ActionType::Act, handle);
						return true;
					});
			}
			break;
			case AbilityTargetingType::SelectSquare:
			{

			}
			default:
				break;
			}
		}
	}
}

void drft::system::HotbarSystem::onUpdateEnd()
{
	registry->clear<component::action::HotbarPressed>();
}
