#include "pch.h"
#include "Biome.h"
#include "Random/RandomNumberGenerator.h"

std::vector<std::string> drft::gen::Biome::pickMachines(int seed) const
{
    std::vector<std::string> result;
    for (auto& [machineName, probability] : possibleMachines)
    {
        const int selection = rng::RandomNumberGenerator::intInRange(0, 1000);
        if ((probability * 1000) > selection)
        {
            result.push_back(machineName);
        }
    }
    return result;
}

std::string drft::gen::Biome::pickRandomEntityFromCategory(const std::string& categoryName) const
{
    auto& category = wildernessPrototypes.at(categoryName);
    const int selection = rng::RandomNumberGenerator::intInRange(0, category.size() - 1);

    return category.at(selection).name;
}
