#pragma once


namespace drft
{
	class EntityFactory;
	struct TilePosition;
}

namespace drft::gen
{
	entt::handle placeSingle(const std::string& name, TilePosition position, entt::registry& registry, const EntityFactory& factory);

	void placeMany(const std::string& name, const std::vector<TilePosition>& positions, entt::registry& reg);

	void placeMany(const std::string& name, TilePosition origin, const std::vector<TilePosition>& positions, entt::registry& reg);

	void placeMany(const std::string& name, TilePosition origin, const std::vector<TilePosition>& positions, entt::registry& reg, std::function<void(entt::handle, sf::Vector3i)> onPlaceFunc);
}