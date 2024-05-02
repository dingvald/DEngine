#pragma once
#include "ISpawningAlgorithm.h"

namespace drft::gen
{
	class NoiseLayerSpawn : public ISpawningAlgorithm
	{
		// Inherited via ISpawningAlgorithm
		virtual PositionList generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const override;
	};
}


