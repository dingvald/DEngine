#include "pch.h"
#include "Logarithmic.h"

float drft::math::log(float x, float a, float b, float c, float d, float e)
{
    return a * (std::logf(b * (x + c)) / std::logf(d)) + e;
}
