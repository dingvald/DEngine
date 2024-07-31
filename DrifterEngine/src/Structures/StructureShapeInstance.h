#pragma once
#include "Spatial/AutoGrid.h"
#include "Utility/stdHashing.h"

namespace drft
{
	class StructureShapeInstance
	{	
	public:
		void addEntity(const std::string& name, sf::Vector2i position);
		void removeEntities(sf::Vector2i position);
		void clearPosition(sf::Vector2i position);
		const std::unordered_map<std::string, std::vector<sf::Vector2i>>& getEntityPositions() const;

		bool hasAnyTag(sf::Vector2i position) const;
		bool hasTag(entt::id_type tag, sf::Vector2i position) const;
		void setTag(entt::id_type tag, sf::Vector2i position);
		void clearTag(entt::id_type tag, sf::Vector2i position);
		bool hasTag(entt::id_type tag) const;
		const std::unordered_set<sf::Vector2i>& getPositionsFor(entt::id_type tag) const;
	private:
		spatial::AutoGrid<std::unordered_set<entt::id_type>> _tagGrid;
		std::unordered_map<entt::id_type, std::unordered_set<sf::Vector2i>> _tagPositions;
		std::unordered_map<std::string, std::vector<sf::Vector2i>> _entityPositions;
	};
}


