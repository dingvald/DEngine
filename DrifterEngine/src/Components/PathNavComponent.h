#pragma once
#include <Spatial/TilePosition.h>
#include <vector>

struct PathNavComponent
{
	std::deque<drft::TilePosition> path;
	size_t progress = 0;
};