#pragma once
#include "GenerationContext.h"
#include "GenerationParameters.h"


namespace drft::gen
{
	using PositionList = std::vector<sf::Vector2i>;
	class ISpawningAlgorithm
	{
	public:
		virtual PositionList generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const = 0;
	};

	void place(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg);
}

