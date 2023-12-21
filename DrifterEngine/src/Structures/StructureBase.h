#pragma once

namespace drft
{
	class StructureBase
	{
	public:
		// Stamps the structure into the registry - returns the area it now occupies.
		virtual sf::IntRect stamp(sf::Vector2i origin, entt::registry& registry) = 0;
		sf::Vector2i getMaximumBounds() const;
		sf::Vector2i getMinimumBounds() const;

	private:
		friend class StructureFactory;
		// For parsing
		void setMaximumBounds(sf::Vector2i bounds);
		void setMinimumBounds(sf::Vector2i bounds);
		void addSubstructure(std::string name, sf::Vector2i relativePosition);

	private:
		struct SubStructurePositionPair
		{
			std::string substructure;
			sf::Vector2i relativePosition;
		};
		std::vector<SubStructurePositionPair> _substructures;
		sf::Vector2i _maxBounds;
		sf::Vector2i _minBounds;
	};
}


