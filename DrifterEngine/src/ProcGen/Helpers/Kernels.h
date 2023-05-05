#pragma once
#include "Spatial/Grid.h"

namespace drft::gen::kernel
{
	const spatial::Grid<int> Cardinal{ 3,3,
		{
			0, 1, 0,
			1, 1, 1,
			0, 1, 0
		}
	};
	

	const spatial::Grid<int> Ordinal{ 3,3,
		{
			1, 1, 1,
			1, 1, 1,
			1, 1, 1
		}
	};
}