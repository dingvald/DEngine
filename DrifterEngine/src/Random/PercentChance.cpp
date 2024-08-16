#include "pch.h"
#include "PercentChance.h"
#include "Random.h"

bool drft::rng::percentChance(int val)
{
    static rng::Random random{ rng::GlobalSeed };
    int choice = random.intInRange(0, 100);
    if (choice <= val) return true;
    return false;
}

bool drft::rng::percentChance(double val)
{
    static rng::Random random{ rng::GlobalSeed };
    float choice = random.realInRange(0.0, 100.0);
    if (choice <= val) return true;
    return false;
}