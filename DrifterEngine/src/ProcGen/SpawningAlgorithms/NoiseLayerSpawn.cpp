#include "pch.h"
#include "NoiseLayerSpawn.h"

drft::gen::PositionList drft::gen::NoiseLayerSpawn::generateSpawnPositions(const GenerationContext& context, const GenerationParameters& params) const
{
	PositionList result;
	const std::string& noiseLayerName = std::get<std::string>(params.at("Layer"));
	const float threshold = std::get<float>(params.at("Threshold"));
	const std::string& comparison = std::get<std::string>(params.at("Comparison"));
	std::function<bool(double, double)> compare;

	if (comparison == "Greater")
	{
		compare = [](double v1, double v2) -> bool {return v1 > v2; };
	}
	else if (comparison == "Less")
	{
		compare = [](double v1, double v2) -> bool {return v1 < v2; };
	}
	else
	{
		throw std::exception("Invalid comparison used.");
	}

	for (int y = 0; y < context.area.height; ++y)
	{
		for (int x = 0; x < context.area.width; ++x)
		{
			double val = context.noiseLayers.at(noiseLayerName).getValueAt({ context.area.left + x, context.area.top + y });
			if (compare(val, threshold) && context.grid.at(context.area.left + x, context.area.top + y).none())
			{
				result.emplace_back(x, y);
			}
		}
	}

	return result;
}
