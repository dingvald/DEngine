#include "pch.h"
#include "CaveBiomeFeature.h"
#include <Spatial/Grid2d.h>
#include <ProcGen/EntityPack/EntityPack.h>
#include <Random/Random.h>

void CaveBiomeFeature::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("radius"))
	{
		_radius.setMin(json["radius"].GetArray()[0].GetFloat());
		_radius.setMax(json["radius"].GetArray()[1].GetFloat());
	}
}

FeatureGenerationResult CaveBiomeFeature::doGenerate(const FeatureGenerationContext& context) const
{
	FeatureGenerationResult result;

	drft::rng::Random random = { context.seed };
	int width = static_cast<int>(random.realInRange(_radius));
	int height = static_cast<int>(random.realInRange(_radius));

	Grid grid = { width, height };
	initGrid(grid, random);
	for (int i = 0; i < _iterations; i++)
	{
		iteration(grid);
	}

	grid.forEach([&](int x, int y, bool val) {
		if (!val) return;
		x = x - width * 0.5f;
		y = y - height * 0.5f;
		result.entityPositions.emplace_back(EMPTY_ENTITY_SLOT, sf::Vector3i{x, y, 0});
		});
	
	return result;
}

int CaveBiomeFeature::sumOfAdjacentWalls(int x, int y, int seachRadius, Grid& grid) const
{
	return 0;
}

void CaveBiomeFeature::initGrid(Grid& grid, drft::rng::Random& random) const
{
	for (int x = 0; x < grid.width(); x++)
	{
		for (int y = 0; y < grid.height(); y++)
		{

		}
	}
}

void CaveBiomeFeature::iteration(Grid& grid) const
{
	for (int x = 0; x < grid.width(); x++)
	{
		for (int y = 0; y < grid.height(); y++)
		{
			int sum = sumOfAdjacentWalls(x, y, 1, grid);

		}
	}
}
