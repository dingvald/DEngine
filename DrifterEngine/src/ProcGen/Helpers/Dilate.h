#pragma once
#include "Kernels.h"

namespace drft::gen
{

spatial::Grid<int> dilate(const spatial::Grid<int>& subject, const spatial::Grid<int>& kernel, int dilatedValue = 1);

}