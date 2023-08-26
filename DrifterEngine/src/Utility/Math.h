#pragma once

namespace drft::math
{
	double lerp(double a, double b, double t);
	double inverseLerp(double a, double b, double v);
	double remap(double iMin, double iMax, double oMin, double oMax, double val);
}