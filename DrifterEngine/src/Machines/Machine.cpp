#include "pch.h"
#include "Components/Components.h"
#include "Machine.h"
#include "ProcGen/Biome.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Conversions.h"
#include "Random/RandomNumberGenerator.h"


void drft::Machine::initialize(entt::registry& registry, const gen::Biome& biome)
{
	_registry = &registry;
	_biome = &biome;
	_factory = &registry.ctx().get<EntityFactory&>();
}

void drft::Machine::setPosition(sf::Vector2i chunkOrigin, sf::Vector2i tilePosition)
{
	_chunkOrigin = chunkOrigin;
	_position = tilePosition;
}

sf::IntRect drft::Machine::getBounds() const
{
	return {_position.x, _position.y, getDimensions().x - 1, getDimensions().y - 1};
}

void drft::Machine::setRelativePosition(entt::handle entity, sf::Vector2i position)
{
	entity.emplace_or_replace<component::Position>(spatial::toWorldSpace(_chunkOrigin + _position + position));
}
