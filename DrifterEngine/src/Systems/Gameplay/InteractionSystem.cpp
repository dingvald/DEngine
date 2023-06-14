#include "pch.h"
#include "InteractionSystem.h"
#include "Components/Components.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"

void drft::system::InteractionSystem::init()
{

}

void drft::system::InteractionSystem::update(const float dt)
{
	const auto& grid = registry->ctx().get<spatial::WorldGrid&>();
	auto interactView = registry->view<const component::Position, component::action::Interact>();
	for (auto [entity, pos] : interactView.each())
	{
		std::vector<entt::entity> canInteractWith;
		auto surroundings = spatial::getIntRectAroundOrigin(spatial::toTileSpace(pos.position), 3, 3);
		for (auto tile : surroundings)
		{
			auto entities = grid.entitiesAt(tile,
				[this](entt::entity entity) -> bool
				{
					return registry->any_of<component::Door>(entity);
				});
			canInteractWith.insert(canInteractWith.end(), entities.begin(), entities.end());
		}
		if (canInteractWith.empty()) continue;
		if (canInteractWith.size() == 1)
		{
			// Easy, just interact with it..
		}
		else
		{
			// Need to let player choose which entity to interact with
		}
	}
}

void drft::system::InteractionSystem::onUpdateEnd()
{
	registry->clear<component::action::Interact>();
}
