#include "pch.h"
#include "Biome.h"
#include "Random/RandomNumberGenerator.h"

std::optional<std::string> drft::gen::Biome::pickMachine(int seed) const
{
    // TODO: make more random - currently picks the first one it finds although many could be "picked"
    const int selection = rng::RandomNumberGenerator::intInRange(0, 1000);
    for (auto& [machineName, probability] : machines)
    {
        if ((probability * 1000) > selection)
        {
            return machineName;
        }
    }
    
    return std::nullopt;
}
