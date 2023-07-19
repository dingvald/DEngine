#pragma once
namespace drft::gen
{
	struct Biome;
}
namespace drft
{
	class EntityFactory;

	class Machine
	{
	public:
		using Ptr = std::unique_ptr<Machine>;

		void initialize(entt::registry& registry, const gen::Biome& biome);
		
		virtual void layout() = 0;

		void setPosition(sf::Vector2i chunkOrigin, sf::Vector2i tilePosition);
		sf::IntRect getBounds() const;

	protected:
		virtual sf::Vector2i getDimensions() const = 0;;
		void setRelativePosition(entt::handle entity, sf::Vector2i position);

	protected:
		struct EntityPositionPair
		{
			std::string entity;
			sf::Vector2i position;
		};
		std::vector<EntityPositionPair> _entities;
		sf::Vector2i _chunkOrigin;
		sf::Vector2i _position;
		entt::registry* _registry = nullptr;
		const EntityFactory* _factory = nullptr;
		const gen::Biome* _biome = nullptr;
	};
}


