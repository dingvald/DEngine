#pragma once
#include <Spatial/TilePosition.h>
#include <vector>

struct PathNavComponent
{
	std::vector<drft::TilePosition> path;
	size_t progress = 0;
};