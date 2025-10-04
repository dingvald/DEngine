#include "pch.h"
#include "Math.h"

bool drft::math::isNear(double d, double target)
{
    return std::abs(d - target) <= std::numeric_limits<double>::epsilon();
}

bool drft::math::isNear(float f, float target)
{
    return std::abs(f - target) <= std::numeric_limits<float>::epsilon();
}

bool drft::math::isNearZero(double d)
{
    return isNear(d, 0.0);
}

bool drft::math::isNearZero(float f)
{
    return isNear(f, 0.0f);
}

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

float drft::math::roundToMultiple(float f, float multiple)
{
    return std::roundf(f / multiple) * multiple;
}

int drft::math::wrap(int val, int min, int max)
{
    if (val < min) val = max;
    if (val > max) val = min;
    return val;
}
