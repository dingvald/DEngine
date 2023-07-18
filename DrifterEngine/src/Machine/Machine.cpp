#include "pch.h"
#include "Components/Components.h"
#include "Machine.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Conversions.h"


void drft::Machine::place(sf::Vector2i tileOrigin, entt::registry& registry) const
{
	const auto& factory = registry.ctx().get<EntityFactory&>();
	for (auto& [position, name] : _entities)
	{
		auto entity = factory.build(name, registry);
		sf::Vector2f worldPosition = spatial::toWorldSpace(tileOrigin + position);
		entity.patch<component::Position>([worldPosition](component::Position& pos) {
				pos.position = worldPosition;
			});
	}
}

sf::Vector2i drft::Machine::getBounds() const
{
	return _bounds;
}

void drft::Machine::addEntity(std::string entityName, sf::Vector2i position)
{
	EntityPositionPair newPair = { position, entityName };
	_entities.emplace_back(newPair);
}

void drft::Machine::setBounds(sf::Vector2i bounds)
{
	_bounds = bounds;
}
