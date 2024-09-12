#include "pch.h"
#include "PlaceEntities.h"
#include "Factory/EntityFactory.h"
#include "Components/PositionComponent.h"
#include <Spatial/Conversions.h>

entt::handle drft::gen::placeSingle(const std::string& name, TilePosition position, entt::registry& registry, const EntityFactory& factory)
{
	auto handle = factory.build(name, registry);
	handle.patch<PositionComponent>([position](PositionComponent& pos) {
			pos.tile = position;
			});
	return handle;
}


void drft::gen::placeMany(const std::string& name, const std::vector<TilePosition>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		placeSingle(name, pos, reg, factory);
	}
}

void drft::gen::placeMany(const std::string& name, TilePosition origin, const std::vector<TilePosition>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		placeSingle(name, origin + pos, reg, factory);
	}
}

void drft::gen::placeMany(const std::string& name, TilePosition origin, const std::vector<TilePosition>& positions, entt::registry& reg, std::function<void(entt::handle, sf::Vector3i)> onPlaceFunc)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		auto handle = placeSingle(name, origin + pos, reg, factory);
		onPlaceFunc(handle, pos);
	}
}
