#include "pch.h"
#include "Biome.h"
#include "Random/RandomNumberGenerator.h"

std::vector<std::string> drft::gen::Biome::pickRandomMachines() const
{
    std::vector<std::string> result;
    for (auto& [machineName, probability] : _machines)
    {
        const int selection = rng::RandomNumberGenerator::intInRange(0, 1000);
        if ((probability * 1000) > selection)
        {
            result.push_back(machineName);
        }
    }
    return result;
}

std::string drft::gen::Biome::pickRandomWildlife(const std::string& category) const
{
    if (!_wildlife.contains(category)) return std::string{};

    std::vector<std::string> entities;
    for (auto& [entity, _] : _wildlife.at(category))
    {
        entities.push_back(entity);
    }
    float sum = 0.f;
    for (auto& entity : entities)
    {
        sum += _wildlife.at(category).at(entity).chance;
    }
    auto rng = std::default_random_engine{};
    std::shuffle(entities.begin(), entities.end(), rng);
    float choice = rng::RandomNumberGenerator::realInRange(0.f, sum);
    for (auto& entity : entities)
    {
        sum -= _wildlife.at(category).at(entity).chance;
        if (sum < choice)
        {
            return entity;
        }
    }

    return std::string{};
}

std::string drft::gen::Biome::pickRandomEnvironmental(const std::string& category) const
{
    if (!_environmentals.contains(category)) return std::string{};

    std::vector<std::string> entities;
    for (auto& [entity, _] : _environmentals.at(category))
    {
        entities.push_back(entity);
    }
    auto rng = std::default_random_engine{};
    std::shuffle(entities.begin(), entities.end(), rng);

    return entities.front();
}

