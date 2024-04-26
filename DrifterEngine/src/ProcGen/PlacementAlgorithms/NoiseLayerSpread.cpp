#include "pch.h"
#include "NoiseLayerSpread.h"

std::vector<sf::Vector2i> drft::gen::noiseLayerSpread(const GenerationContext& ctx, const GenerationParameters& params)
{
	std::vector<sf::Vector2i> result;
    const std::string& noiseLayerName = std::get<std::string>(params.at("Layer"));
    const float threshold = std::get<float>(params.at("Threshold"));

	for (int y = 0; y < ctx.area.height; ++y)
	{
		for (int x = 0; x < ctx.area.width; ++x)
		{
			double val = ctx.noiseLayers.at(noiseLayerName).getValueAt({ x, y });
			if (val < threshold && ctx.grid.at(ctx.area.left + x, ctx.area.top + y).none())
			{
				result.emplace_back(x, y);
			}
		}
	}

    return std::vector<sf::Vector2i>();
}
