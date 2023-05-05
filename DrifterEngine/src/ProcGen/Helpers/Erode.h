#pragma once
#include "Kernels.h"

namespace drft::gen
{
	spatial::Grid<int> erode(const spatial::Grid<int>& subject, const spatial::Grid<int>& kernel, int erodedValue = 0);
}