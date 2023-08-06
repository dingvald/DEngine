#include "pch.h"
#include "InteractionSystem.h"
#include "Components/Components.h"
#include "Events/RequestStateChange.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Utility/EntityHelpers.h"

void drft::system::InteractionSystem::init()
{
	registry->on_construct<component::action::DoInteract>().connect<&InteractionSystem::onContructDoInteract>(this);
}

void drft::system::InteractionSystem::update(const float dt)
{
	const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	auto interactView = registry->view<const component::Position, component::action::TryInteract>();
	for (auto [entity, pos] : interactView.each())
	{
		std::vector<entt::entity> canInteractWith;
		auto surroundings = spatial::getIntRectAroundOrigin(pos.position, 3, 3);
		for (auto tile : surroundings)
		{
			auto entities = grid.entitiesAt(tile, 
				[this](entt::entity entity) -> bool {
					return registry->any_of < component::Usable > (entity);
				});
			canInteractWith.insert(canInteractWith.end(), entities.begin(), entities.end());
		}
		if (canInteractWith.size() == 1)
		{
			auto interactable = canInteractWith.front();
			registry->emplace<component::action::DoInteract>(entity, entity, interactable);
		}
		else if (canInteractWith.size() > 1)
		{
			auto tilePosition = pos.position;
			registry->emplace<component::action::SelectDirection>(entity,
				[this, tilePosition, entity](sf::Vector2i direction) -> bool
				{
					const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
					auto interactables = grid.entitiesAt(tilePosition + direction, 
						[this](entt::entity entity) -> bool {
							return registry->any_of<component::Usable>(entity);
						});
					if (interactables.empty()) return false;
					for (auto interactable : interactables)
					{
						registry->emplace<component::action::DoInteract>(entity, entity, interactable);
					}
					return true;
				});
		}
	}
}

void drft::system::InteractionSystem::onUpdateEnd()
{
	registry->clear<component::action::TryInteract>();
	registry->clear<component::action::DoInteract>();
}

void drft::system::InteractionSystem::onTargetSelected(entt::entity actor, sf::Vector2i target)
{
	const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	auto entities = grid.entitiesAt(target, [this](entt::entity entity) -> bool {
		return registry->any_of<component::Usable>(entity);
		});
	for (auto entity : entities)
	{
		registry->emplace<component::action::DoInteract>(actor, actor, entity);
	}
}

void drft::system::InteractionSystem::onContructDoInteract(entt::registry& registry, entt::entity entity)
{
	auto interaction = registry.get<component::action::DoInteract>(entity);
	if (auto item = registry.try_get<component::Item>(interaction.subject))
	{
		registry.emplace_or_replace<component::action::Use>(entity, interaction.subject, item->id);
	}
	else
	{
		registry.emplace_or_replace<component::action::Use>(entity, interaction.subject, component::Item::NONE);
	}	
}
