#include "pch.h"
#include "Math.h"

double drft::math::lerp(double a, double b, double t)
{
    return (1.0 - t) * a + b * t;
}

double drft::math::inverseLerp(double a, double b, double v)
{
    return (v - a) / (b - a);
}

double drft::math::remap(double iMin, double iMax, double oMin, double oMax, double val)
{
    return lerp(oMin, oMax, inverseLerp(iMin, iMax, val));
}

float drft::math::floorToMultiple(float f, float multiple)
{
    return std::floorf(f / multiple) * multiple;
}

int drft::math::wrap(int val, int min, int max)
{
    if (val < min) val = max;
    if (val > max) val = min;
    return val;
}
