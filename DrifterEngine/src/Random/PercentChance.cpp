#include "pch.h"
#include "PercentChance.h"
#include "RandomNumberGenerator.h"

bool drft::rng::percentChance(int val)
{
    int choice = rng::RandomNumberGenerator::intInRange(0, 100);
    if (choice <= val) return true;
    return false;
}
