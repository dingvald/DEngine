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
}

