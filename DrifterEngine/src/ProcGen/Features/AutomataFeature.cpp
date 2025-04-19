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
	if (json.HasMember("on_tags"))
	{
		for (auto&& tag : json["on_tags"].GetArray())
		{
			_onTags.emplace_back(entt::hashed_string{ tag.GetString() });
		}
	}
	if (json.HasMember("off_tags"))
	{
		for (auto&& tag : json["off_tags"].GetArray())
		{
			_offTags.emplace_back(entt::hashed_string{ tag.GetString() });
		}
	}
	if (json.HasMember("center_tags"))
	{
		for (auto&& tag : json["center_tags"].GetArray())
		{
			_centerTags.emplace_back(entt::hashed_string{ tag.GetString() });
		}
	}
	if (json.HasMember("border_tags"))
	{
		for (auto&& tag : json["border_tags"].GetArray())
		{
			_borderTags.emplace_back(entt::hashed_string{ tag.GetString() });
		}
	}
	if (json.HasMember("radius"))
	{
		_radius.setMin(json["radius"].GetArray()[0].GetInt());
		_radius.setMax(json["radius"].GetArray()[1].GetInt());
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
	const int width = random.intInRange(_radius);
	const int height = random.intInRange(_radius);

	Grid grid = { width, height };
	initGrid(grid, random);
	for (int i = 0; i < _iterations; i++)
	{
		iteration(grid);
	}

	const int halfWidth = width * 0.5f;
	const int halfHeight = height * 0.5f;

	for (auto&& tag : _centerTags)
	{
		result[tag].push_back(sf::Vector3i{0,0,0});
	}

	grid.forEach([&](int x, int y, bool val) {
		sf::Vector3i position = { x - halfWidth, y - halfHeight, 0 };
		if (!val && !_offTags.empty())
		{
			for (auto&& tag : _offTags)
			{
				result[tag].push_back(position);
			}
		}
		if (val && !_onTags.empty())
		{
			for (auto&& tag : _onTags)
			{
				result[tag].push_back(position);
			}
		}
		if (!_borderTags.empty())
		{
			auto neighbors = drft::spatial::getAdjacentPoints({ x, y });
			for (auto&& neighbor : neighbors)
			{
				if (!grid.contains(neighbor.x, neighbor.y)) continue;
				if (grid.at(neighbor.x, neighbor.y) == val) continue;
				for (auto&& tag : _borderTags)
				{
					result[tag].push_back(position);
				}
			}
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

