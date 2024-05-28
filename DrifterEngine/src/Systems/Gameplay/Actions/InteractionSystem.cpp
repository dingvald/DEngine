#include "pch.h"
#include "InteractionSystem.h"

#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/InteractableComponent.h"

#include "Events/RequestStateChange.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Utility/EntityHelpers.h"

void drft::system::InteractionSystem::init()
{
	_registry->on_construct<component::action::DoInteract>().connect<&InteractionSystem::onContructDoInteract>(this);
}

void drft::system::InteractionSystem::update(const float dt)
{
	const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	auto interactView = _registry->view<const PositionComponent, component::action::TryInteract>();
	for (auto [entity, pos] : interactView.each())
	{
		auto usableEntities = getInteractableSurroundings(pos.position, grid);
		if (usableEntities.size() == 1)
		{
			_registry->emplace<component::action::DoInteract>(entity, std::move(usableEntities));
		}
		else if (usableEntities.size() > 1)
		{
			auto tilePosition = pos.position;
			_registry->emplace<component::action::SelectDirection>(entity,
				[this, tilePosition, entity](sf::Vector2i direction) -> bool
				{
					sf::Vector2i targetPosition = tilePosition + direction;
					return onTargetSelected(entity, targetPosition);
				});
		}
	}
}

void drft::system::InteractionSystem::onUpdateEnd()
{
	_registry->clear<component::action::TryInteract>();
	_registry->clear<component::action::DoInteract>();
}

std::vector<entt::entity> drft::system::InteractionSystem::getInteractableSurroundings(sf::Vector2i position, const spatial::WorldGrid& grid)
{
	std::vector<entt::entity> result;
	auto surroundings = spatial::getIntRectAroundOrigin(position, 3, 3);
	for (auto&& tile : surroundings)
	{
		auto entities = grid.entitiesAt(tile,
			[this](entt::entity entity) -> bool 
			{
				return _registry->any_of<InteractableComponent>(entity);
			});
		result.insert(result.end(), entities.begin(), entities.end());
	}
	return result;
}

bool drft::system::InteractionSystem::onTargetSelected(entt::entity actor, sf::Vector2i target)
{
	const auto& grid = _registry->ctx().get<spatial::WorldGrid&>();
	auto entities = grid.entitiesAt(target, [this](entt::entity entity) -> bool 
		{
			return _registry->any_of<InteractableComponent>(entity);
		});

	if (entities.empty()) return false;

	_registry->emplace<component::action::DoInteract>(actor, std::move(entities));

	return true;
}

void drft::system::InteractionSystem::onContructDoInteract(entt::registry& registry, entt::entity entity)
{
	auto& interactionAction = registry.get<component::action::DoInteract>(entity);
	const auto& actorName = util::getEntityName({ registry, entity });
	for (auto&& subject : interactionAction.subjects)
	{
		const auto& subjectName = util::getEntityName({ registry, subject });
		if (auto interactable = registry.try_get<InteractableComponent>(subject))
		{
			if (interactable->interactions.size() == 1)
			{
				auto&& [name, interactionFunction] = *interactable->interactions.begin();
				std::cout << actorName << " " << name << "s " << subjectName << "." << std::endl;
				interactionFunction(entity, subject);
			}
			else
			{
				// TODO: handle if one object can be interacted with in multiple ways
			}
		}
	}
}
