#include "pch.h"
#include "Biome.h"
#include "Random/RandomNumberGenerator.h"

std::vector<std::string> drft::gen::Biome::pickRandomMachines() const
{
    std::vector<std::string> result;
    for (auto& [machineName, machineObject] : _machines)
    {
        const int selection = rng::RandomNumberGenerator::intInRange(0, 1000);
        if ((machineObject.chance * 1000) > selection)
        {
            result.push_back(machineName);
        }
    }
    return result;
}

std::optional<std::string> drft::gen::Biome::pickRandomWildlife(std::vector<std::pair<std::string, float>> categories) const
{
    float sum = 0.f;
    for (auto&& [cat, weight] : categories)
    {
        sum += weight;
    }

    float choice = rng::RandomNumberGenerator::realInRange(0, sum);
    auto rng = std::default_random_engine{};
    std::shuffle(categories.begin(), categories.end(), rng);
    std::string selectedCategory = "";
    for (auto&& [cat, weight] : categories)
    {
        sum -= weight;
        if (_wildlife.contains(cat) && choice > sum)
        {
            selectedCategory = cat;
            break;
        }
    }
    if (selectedCategory.length() > 0)
    {
        return _pickRandomWildlife(selectedCategory);
    }

    return std::nullopt;
}

std::optional<std::string> drft::gen::Biome::pickRandomEnvironmental(std::vector<std::string> categories) const
{
    auto rng = std::default_random_engine{};
    std::shuffle(categories.begin(), categories.end(), rng);
    std::string selectedCategory = "";
    for (int i = 0; i < categories.size(); ++i)
    {
        if (_environmentals.contains(categories.at(i)))
        {
            selectedCategory = categories.at(i);
            break;
        }
    }
    if (selectedCategory.length() > 0)
    {
        return _pickRandomEnvironmental(selectedCategory);
    }

    return std::nullopt;
}

std::optional<std::string> drft::gen::Biome::_pickRandomWildlife(const std::string& category) const
{
    if (!_wildlife.contains(category)) return std::nullopt;

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

    return std::nullopt;
}

std::optional<std::string> drft::gen::Biome::_pickRandomEnvironmental(const std::string& category) const
{
    if (!_environmentals.contains(category)) return std::nullopt;

    std::vector<std::string> entities;
    for (auto& [entity, _] : _environmentals.at(category))
    {
        entities.push_back(entity);
    }
    auto rng = std::default_random_engine{};
    std::shuffle(entities.begin(), entities.end(), rng);

    return entities.front();
}

