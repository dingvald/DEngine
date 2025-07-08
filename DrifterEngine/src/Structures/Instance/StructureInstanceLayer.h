#pragma once
#include <Spatial/Grid2d.h>

class StructureInstanceLayer
{
public:

private:
	using EntitySlotGrid = drft::spatial::Grid2d<entt::id_type>;
	EntitySlotGrid _slotGrid;
};