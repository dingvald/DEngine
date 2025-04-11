#include "pch.h"
#include "BiomeLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

#include <Random/Random.h>
#include <Spatial/Helpers.h>
#include <Utility/stdHashing.h>


using namespace entt::literals;
using namespace drft;


GenerationState BiomeLayerChunk::generate(GenerationLevel)
{
    const auto paddedVolume = addPaddingToVolume({ _volume.dimensions().x, _volume.dimensions().y, 0 });

    return assignBiomesToVoronoiCells(paddedVolume);
}

void BiomeLayerChunk::assignBiomeToVoronoiCell(sf::Vector3i centroid, const Biome::DependencyValues& climateValues)
{
    std::vector<const Biome*> potentialBiomes;
    std::map<float, const Biome*> rankings;
    for (auto&& biome : _layer.getBiomes())
    {
        if (!biome) continue;
        if (biome->satisfiesClimate(climateValues))
        {
            potentialBiomes.push_back(biome);
        }
        rankings.emplace(biome->closenessToClimate(climateValues), biome);
    }

    auto point = spatial::toXY(centroid);
    biomePositions.push_back(point);

    if (potentialBiomes.size() == 1)
    {
        biomePoints.emplace(point, potentialBiomes.front());
    }
    else if (potentialBiomes.size() > 1)
    {
        drft::rng::Random random{ getGlobalSeed() + std::hash<sf::Vector3i>()(centroid)};
        size_t index = random.intInRange(0, potentialBiomes.size());
        biomePoints.emplace(point, potentialBiomes.at(index));
    }
    else if (!rankings.empty())
    {
        biomePoints.emplace(point, rankings.begin()->second);
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

    const auto centroids = voronoiLayer.unwrap().getCentroidsInArea(volume.flatten(), volume.center());
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
        const auto closestBiomePosition = drft::spatial::findClosestPoint2d(spatial::toXY(tilePosition), chunk->biomePositions);
        if (chunk->biomePoints.contains(closestBiomePosition))
        {
            return chunk->biomePoints.at(closestBiomePosition);
        }
    }
    return nullptr;
}

void drft::BiomeLayer::forEachBiomeInArea(sf::IntRect area, std::function<void(const Biome* biome)> func)
{
    auto chunks = getChunkPointsInsideArea(area)
}

sf::Vector3i drft::BiomeLayer::getChunkDimensions() const
{
    return { 8, 8, 8 };
}
