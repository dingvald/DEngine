#pragma once
#include "StructureBase.h"

namespace drft
{
	class StaticStructure : public StructureBase
	{
	public:
		virtual sf::IntRect stamp(sf::Vector2i origin, entt::registry& registry) override;
	private:
		bool canMirror = false;
		bool canRotate = false;
	};
}


