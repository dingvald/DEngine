#include "pch.h"
#include "BiomeLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

#include <Random/Random.h>
#include <Spatial/Helpers.h>
#include <Utility/stdHashing.h>
#include <Utility/ContainerHelpers.h>


using namespace entt::literals;
using namespace drft;


GenerationState BiomeLayerChunk::generate(GenerationLevel)
{
    return assignBiomesToVoronoiCells(_volume.expand({2.f, 2.f, 1.f}));
}

void BiomeLayerChunk::assignBiomeToVoronoiCell(sf::Vector3i centroid, const Biome::DependencyValues& climateValues)
{
    std::vector<const Biome*> satisfyingBiomes;
    std::map<float, const Biome*> biomeRankings;

    // Rank biomes
    for (auto&& biome : _layer.getBiomes())
    {
        if (!biome) continue;
        if (biome->satisfiesClimate(climateValues))
        {
            satisfyingBiomes.push_back(biome);
        }
        biomeRankings.emplace(biome->closenessToClimate(climateValues), biome);
    }

    auto point = spatial::toXY(centroid);
    // Prioritize satisfying biomes
    if (satisfyingBiomes.size() == 1)
    {
        biomePoints.emplace(point, satisfyingBiomes.front());
    }
    else if (satisfyingBiomes.size() > 1)
    {
        drft::rng::Random random{ getGlobalSeed() + std::hash<sf::Vector3i>()(centroid)};
        size_t index = random.intInRange(0, satisfyingBiomes.size());
        biomePoints.emplace(point, satisfyingBiomes.at(index));
    }
    // Fall back on their rank
    else if (!biomeRankings.empty())
    {
        biomePoints.emplace(point, biomeRankings.begin()->second);
    }
}

Biome::DependencyValues BiomeLayerChunk::getClimateValuesAtPoint(sf::Vector3i point, const GeneratedDependencies& generatedDependencies) const
{
    Biome::DependencyValues result;
    for (auto&& [name, layerPtr] : generatedDependencies)
    {
        if (!layerPtr) continue; // TODO: Should maybe be an assert...?
        result.emplace(name, layerPtr->getValueAt(point));
    }
    return result;
}

GenerationState BiomeLayerChunk::assignBiomesToVoronoiCells(spatial::AABB<int> volume)
{
    auto voronoiLayer = generateDependency<VoronoiLayer>(volume);
    if (!voronoiLayer.isReady()) return voronoiLayer.getState();

    GeneratedDependencies generatedDependencies;
    for (auto&& dependencyID : _layer.getClimateDependencies())
    {
        auto depLayer = generateDependency<IGetValueAtLayer>(dependencyID, volume);
        if (!depLayer.isReady()) return depLayer.getState();

        generatedDependencies.emplace(dependencyID, &depLayer.unwrap());
    }

    const auto centroids = voronoiLayer.unwrap().getCentroidsInArea(volume.flatten(), volume.center().z);
    for (auto&& point : centroids)
    {
        const auto values = getClimateValuesAtPoint(point, generatedDependencies);
        assignBiomeToVoronoiCell(point, values);
    }

    return GenerationState::Complete;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///     BiomeLayer     //////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drft::BiomeLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("biomes"))
    {
        for (auto&& val : json["biomes"].GetArray())
        {
            std::string biomeName = val.GetString();
            const Biome* biome = getRegistries().biomes.get(biomeName);
            if (!biome)
            {
                LOG_WARNING("Biome name {} does not exist in the biome registry", biomeName);
                continue;
            }
            
            auto climateDependencies = biome->getClimateDependencyIds();
            _climateDependencies.insert(climateDependencies.begin(), climateDependencies.end());

            _biomes.push_back(biome);
        }
    }
}

const std::vector<const Biome*>& drft::BiomeLayer::getBiomes() const
{
    return _biomes;
}

const std::unordered_set<entt::id_type>& BiomeLayer::getClimateDependencies() const
{
    return _climateDependencies;
}

const Biome* BiomeLayer::getBiomeAt(sf::Vector3i tilePosition) const
{
    if (const auto chunk = tryGetChunk(tilePosition))
    {
        if (chunk->biomePoints.empty()) return nullptr;
        const auto closestBiomePosition = drft::spatial::findClosestPoint2d(spatial::toXY(tilePosition), util::extractKeys(chunk->biomePoints));
        return chunk->biomePoints.at(closestBiomePosition);
    }
    return nullptr;
}

void drft::BiomeLayer::forEachBiomeInArea(sf::IntRect area, int z, std::function<void(const Biome* biome)> func)
{
    std::unordered_set<const Biome*> biomes;
    forEachLoadedChunkInArea(area, z, [&biomes, &area, &func](BiomeLayerChunk& chunk) {
        for (auto&& [point, biome] : chunk.biomePoints)
        {
            if (!area.contains(point)) continue;
            if (biomes.contains(biome)) continue;
            func(biome);
            biomes.insert(biome);
        }
    });
}

sf::Vector3i drft::BiomeLayer::getChunkDimensions() const
{
    return { 8, 8, 8 };
}
