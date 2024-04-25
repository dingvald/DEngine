#pragma once

namespace drft
{
	class StructureBase
	{
	public:
		StructureBase(std::string name)
			: _name(name) {}
		// Stamps the structure into the registry - returns the area it now occupies.
		virtual sf::IntRect stamp(sf::Vector2i origin, entt::registry& registry) const = 0;
		sf::Vector2i getMaximumBounds() const;
		sf::Vector2i getMinimumBounds() const;
		const std::string& name() const;

	private:
		friend class StructureRegistry;
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
		std::string _name;
		std::vector<SubStructurePositionPair> _substructures;
		sf::Vector2i _maxBounds;
		sf::Vector2i _minBounds;
	};
}


