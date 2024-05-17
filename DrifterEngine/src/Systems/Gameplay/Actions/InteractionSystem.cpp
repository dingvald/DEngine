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
		const auto usableEntities = getUsableSurroundings(pos.position, grid);
		if (usableEntities.size() == 1)
		{
			auto interactable = usableEntities.front();
			registry->emplace<component::action::DoInteract>(entity, entity, interactable);
		}
		else if (usableEntities.size() > 1)
		{
			auto tilePosition = pos.position;
			registry->emplace<component::action::SelectDirection>(entity,
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
	registry->clear<component::action::TryInteract>();
	registry->clear<component::action::DoInteract>();
}

std::vector<entt::entity> drft::system::InteractionSystem::getUsableSurroundings(sf::Vector2i position, const spatial::WorldGrid& grid)
{
	std::vector<entt::entity> result;
	auto surroundings = spatial::getIntRectAroundOrigin(position, 3, 3);
	for (auto&& tile : surroundings)
	{
		auto entities = grid.entitiesAt(tile,
			[this](entt::entity entity) -> bool {
				return registry->any_of < component::Usable >(entity);
			});
		result.insert(result.end(), entities.begin(), entities.end());
	}
	return result;
}

bool drft::system::InteractionSystem::onTargetSelected(entt::entity actor, sf::Vector2i target)
{
	const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	auto entities = grid.entitiesAt(target, [this](entt::entity entity) -> bool {
		return registry->any_of<component::Usable>(entity);
		});
	if (entities.empty()) return false;
	for (auto entity : entities)
	{
		registry->emplace<component::action::DoInteract>(actor, actor, entity);
	}
	return true;
}

void drft::system::InteractionSystem::onContructDoInteract(entt::registry& registry, entt::entity entity)
{
	auto& interaction = registry.get<component::action::DoInteract>(entity);
	if (auto item = registry.try_get<component::Item>(interaction.subject))
	{
		registry.emplace_or_replace<component::action::Use>(entity, interaction.subject, item->id);
	}
	else
	{
		registry.emplace_or_replace<component::action::Use>(entity, interaction.subject, component::Item::NONE);
	}	
}
