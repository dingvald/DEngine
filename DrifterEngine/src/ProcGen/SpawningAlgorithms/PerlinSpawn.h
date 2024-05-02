#pragma once
#include "ISpawningAlgorithm.h"

namespace drft::gen
{
	class PerlinSpawn : public ISpawningAlgorithm
	{
		// Inherited via ISpawningAlgorithm
		virtual PositionList generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const override;
	};

}

