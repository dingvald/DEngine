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

std::string drft::gen::Biome::pickRandomEntity(const std::string& category) const
{
    auto& prototype = prototypes.at(category);
    auto choice = rng::RandomNumberGenerator::intInRange(0, prototype.size() - 1);

    return prototype[choice].name;
}
