#pragma once

namespace drft
{
	class Machine
	{
	public:
		void place(sf::Vector2i tileOrigin, entt::registry& registry) const;
		sf::Vector2i getBounds() const;

	protected:
		friend class MachineFactory;
		struct EntityPositionPair
		{
			sf::Vector2i position;
			std::string entity;
		};
		void addEntity(std::string entityName, sf::Vector2i position);
		void setBounds(sf::Vector2i bounds);

	private:
		std::vector<EntityPositionPair> _entities;
		sf::Vector2i _bounds = { 0,0 };
	};
}


