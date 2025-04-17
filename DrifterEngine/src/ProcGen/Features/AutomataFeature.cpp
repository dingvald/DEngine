#include "pch.h"
#include "AutomataFeature.h"
#include <Spatial/Grid2d.h>
#include <Spatial/Helpers.h>
#include <Random/Random.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <ProcGen/GenerationContext.h>

using namespace entt::literals;

void AutomataFeature::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("tags"))
	{
		for (auto&& tag : json["tags"].GetArray())
		{
			_tags.emplace_back(entt::hashed_string{ tag.GetString() });
		}
	}
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

TaggedPositions AutomataFeature::doGenerate(const GenerationContext& context) const
{
	TaggedPositions result;

	drft::rng::Random random = { context.seed };
	int width = static_cast<int>(random.realInRange(_radius));
	int height = static_cast<int>(random.realInRange(_radius));

	Grid grid = { width, height };
	initGrid(grid, random);
	for (int i = 0; i < _iterations; i++)
	{
		iteration(grid);
	}

	const int halfWidth = width * 0.5f;
	const int halfHeight = height * 0.5f;

	grid.forEach([&](int x, int y, bool val) {
		if (!val) return;
		sf::Vector3i position = { x - halfWidth, y - halfHeight, 0 };
		for (auto&& tag : _tags)
		{
			result[tag].push_back(position);
		}
	});
	
	return result;
}

int AutomataFeature::sumOfAdjacentCells(int x, int y, int searchRadius, Grid& grid) const
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
	return result;
}

void AutomataFeature::initGrid(Grid& grid, drft::rng::Random& random) const
{
	for (int x = 0; x < grid.width(); x++)
	{
		for (int y = 0; y < grid.height(); y++)
		{
			grid.at(x, y) = (random.realInRange(0.0f, 1.0f) > 0.5f);
		}
	}
}

void AutomataFeature::iteration(Grid& grid) const
{
	for (int x = 0; x < grid.width(); x++)
	{
		for (int y = 0; y < grid.height(); y++)
		{
			int sum = sumOfAdjacentCells(x, y, 1, grid);
			grid.at(x, y) = grid.at(x, y) ? (sum >= 3) : (sum >= 5 || sumOfAdjacentCells(x, y, 2, grid) <= 2);
		}
	}
}

