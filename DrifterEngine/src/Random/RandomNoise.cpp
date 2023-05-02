#include "pch.h"
#include "RandomNoise.h"

unsigned int drft::rng::noise(int position)
{
    return std::hash<int>{}(position);
}
