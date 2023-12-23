#pragma once
#include "StructureBase.h"

namespace drft
{
	class StaticStructure : public StructureBase
	{
	public:
		StaticStructure(std::string name)
			: StructureBase(name) {}
		virtual sf::IntRect stamp(sf::Vector2i origin, entt::registry& registry) const override;

		// for parsing
		void addEntity(const std::string& entity, std::vector<sf::Vector2i> positions);
		void setCanMirror(bool val);
		void setCanRotate(bool val);
	private:
		bool _canMirror = false;
		bool _canRotate = false;
		std::unordered_map<std::string, std::vector<sf::Vector2i>> _entities;
	};
}


