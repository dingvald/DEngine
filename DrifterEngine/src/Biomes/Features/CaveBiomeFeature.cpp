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
	if (json.HasMember("iterations"))
	{
		_iterations = json["iterations"].GetInt();
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

int CaveBiomeFeature::sumOfAdjacentWalls(int x, int y, int searchRadius, Grid& grid) const
{
	int result = 0;
	for (int ix = x - searchRadius; ix <= x + searchRadius; ix++)
	{
		for (int iy = y - searchRadius; iy <= y + searchRadius; iy++)
		{
			if (!grid.contains(ix, iy)) continue;
			if (ix == x && iy == y) continue;
			if (grid.at(ix, iy)) result++;
		}
	}
	return 0;
}

void CaveBiomeFeature::initGrid(Grid& grid, drft::rng::Random& random) const
{
	for (int x = 0; x < grid.width(); x++)
	{
		for (int y = 0; y < grid.height(); y++)
		{
			grid.at(x, y) = (random.realInRange(0.0f, 1.0f) > 0.5f);
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
			grid.at(x, y) = grid.at(x, y) ? (sum >= 3) : (sum >= 5 || sumOfAdjacentWalls(x, y, 2, grid) <= 2);
		}
	}
}
