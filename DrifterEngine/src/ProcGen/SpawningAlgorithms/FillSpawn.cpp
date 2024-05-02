#include "pch.h"
#include "FillSpawn.h"

drft::gen::PositionList drft::gen::FillSpawn::generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const
{
	PositionList positions;
	positions.reserve(context.area.width * context.area.height);

	for (int y = 0; y < context.area.height; ++y)
	{
		for (int x = 0; x < context.area.width; ++x)
		{
			if (context.grid.at(context.area.left + x, context.area.top + y).any()) continue;
			positions.emplace_back(x, y);
		}
	}

	return positions;
}
