#include "pch.h"
#include "NoiseLayerSpread.h"

std::vector<sf::Vector2i> drft::gen::noiseLayerSpread(const GenerationContext& ctx, const GenerationParameters& params)
{
	std::vector<sf::Vector2i> result;
    const std::string& noiseLayerName = std::get<std::string>(params.at("Layer"));
    const float threshold = std::get<float>(params.at("Threshold"));
	const std::string& comparison = std::get<std::string>(params.at("Comparison"));
	std::function<bool(double, double)> compare;

	if (comparison == "Greater")
	{
		compare = [](double v1, double v2) -> bool{return v1 > v2;};
	}
	else if (comparison == "Less")
	{
		compare = [](double v1, double v2) -> bool {return v1 < v2; };
	}
	else
	{
		throw std::exception("Invalid comparison used.");
	}

	for (int y = 0; y < ctx.area.height; ++y)
	{
		for (int x = 0; x < ctx.area.width; ++x)
		{
			double val = ctx.noiseLayers.at(noiseLayerName).getValueAt({ ctx.area.left + x, ctx.area.top + y });
			if (compare(val, threshold) && ctx.grid.at(ctx.area.left + x, ctx.area.top + y).none())
			{
				result.emplace_back(x, y);
			}
		}
	}

    return result;
}
