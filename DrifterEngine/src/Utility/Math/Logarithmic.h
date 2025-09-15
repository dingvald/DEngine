#pragma once

namespace drft::math
{
	// exands to [ a * (log(b*(x+c)) / log(d)) + e ]
	float log(float x, float a, float b, float c, float d, float e);
}