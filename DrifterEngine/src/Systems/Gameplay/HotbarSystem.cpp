#include "pch.h"
#include "HotbarSystem.h"

#include "Components/Components.h"
#include "Components/PlayerComponent.h"
#include "Components/HotbarComponent.h"
#include "Components/PositionComponent.h"

#include "Components/Tags.h"
#include "Ability/AbilityRegistry.h"
#include "Systems/Helpers/SpendActionPoints.h"
#include "Systems/Helpers/ToHotbarIndex.h"

#include <Spatial/Conversions.h>
#include <Spatial/Helpers.h>

void drft::system::HotbarSystem::init()
{
	AbilityRegistry::bind();
}

void drft::system::HotbarSystem::onStart()
{
	auto playerView = _registry.view<PlayerComponent>();
	for (auto entity : playerView)
	{
		if (_registry.all_of<HotbarComponent>(entity)) continue;

		auto& hotbar = _registry.emplace<HotbarComponent>(entity);
		// TODO: For testing purposes only
		hotbar.abilities[toHotbarIndex(1)] = AbilityType::Sprint;
		hotbar.abilities[toHotbarIndex(2)] = AbilityType::Throw;
	}
}

void drft::system::HotbarSystem::onUpdate(float dt)
{
	auto view = _registry.view<HotbarComponent, component::action::HotbarPressed, component::tag::CurrentActor>();
	for (auto&& [entity, hotbar, hotbarSlot] : view.each())
	{
		entt::handle handle = { _registry, entity };
		AbilityType abilityType = hotbar.abilities[hotbarSlot.slot];
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
					auto tilePosition = handle.get<PositionComponent>().tile;
					handle.emplace<component::action::SelectDirection>(
						[tilePosition, &ability, &handle](sf::Vector2i direction) -> bool
						{
							ability.perform(handle, tilePosition + spatial::asTileSpace(direction));
							spendActionPoints(ability.getCost(), ActionType::Act, handle);
							return true;
						});
				}
				break;
				case AbilityTargetingType::SelectSquare:
				{
					auto range = ability.getRange(handle);
					auto targetingShape = ability.getTargetingShape(handle);
					handle.emplace<component::action::SelectTarget>(range, targetingShape,
						[&ability, handle](sf::Vector3i position) -> bool {
							ability.perform(handle, spatial::asTileSpace(position));
							spendActionPoints(ability.getCost(), ActionType::Act, handle);
							return true;
						});
				}
				break;
				default:
					throw std::exception("Unhandled targetting type for ability.");
					break;
			}
		}
	}
}

void drft::system::HotbarSystem::onUpdateEnd()
{
	_registry.clear<component::action::HotbarPressed>();
}
