#include "pch.h"
#include "InteractionSystem.h"

#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/InteractableComponent.h"
#include "Components/Actions/InteractionAction.h"

#include "Events/RequestStateChange.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Utility/EntityHelpers.h"

void drft::system::InteractionSystem::init()
{
	_registry.on_construct<InteractionAction>().connect<&InteractionSystem::onConstructInteractionAction>(this);
}

void drft::system::InteractionSystem::onUpdateEnd()
{
	_registry.clear<InteractionAction>();
}

std::vector<entt::entity> drft::system::InteractionSystem::getInteractableSurroundings(sf::Vector2i position, const spatial::WorldGrid& grid)
{
	std::vector<entt::entity> result;
	auto surroundings = spatial::getAdjacentPoints(position);
	for (auto&& tile : surroundings)
	{
		auto entities = grid.entitiesAt(tile,
			[this](entt::entity entity) -> bool 
			{
				return _registry.any_of<InteractableComponent>(entity);
			});
		result.insert(result.end(), entities.begin(), entities.end());
	}
	return result;
}

void drft::system::InteractionSystem::doInteract(entt::entity actor, const std::vector<entt::entity>& interactables) const
{
	for (auto&& interactable : interactables)
	{
		if (auto interactableComponent = _registry.try_get<InteractableComponent>(interactable))
		{
			if (interactableComponent->interactions.size() == 1)
			{
				auto&& [name, interactionFunction] = *interactableComponent->interactions.begin();
				interactionFunction(actor, interactable);
			}
			else
			{
				// TODO: handle if one object can be interacted with in multiple ways
			}
		}
	}
	
}

bool drft::system::InteractionSystem::onTargetSelected(entt::entity actor, sf::Vector2i target)
{
	const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
	auto entities = grid.entitiesAt(target, [this](entt::entity entity) -> bool 
		{
			return _registry.any_of<InteractableComponent>(entity);
		});

	if (entities.empty()) return false;

	doInteract(actor, entities);

	return true;
}

void drft::system::InteractionSystem::onConstructInteractionAction(entt::registry& registry, entt::entity entity)
{
	if (auto positionComponent = registry.try_get<PositionComponent>(entity))
	{
		const auto& grid = _registry.ctx().get<spatial::WorldGrid&>();
		auto usableEntities = getInteractableSurroundings(positionComponent->position, grid);
		if (usableEntities.size() == 1)
		{
			doInteract(entity, usableEntities);
		}
		else if (usableEntities.size() > 1)
		{
			auto tilePosition = positionComponent->position;
			_registry.emplace<component::action::SelectDirection>(entity,
				[this, tilePosition, entity](sf::Vector2i direction) -> bool
				{
					sf::Vector2i targetPosition = tilePosition + direction;
					return onTargetSelected(entity, targetPosition);
				});
		}
	}
}
