#include "pch.h"
#include "BiomeLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

#include <Random/Random.h>
#include <Spatial/Helpers.h>
#include <Utility/ContainerHelpers.h>
#include <Utility/stdHashing.h>

using namespace entt::literals;
using namespace drft;

GenerationState BiomeLayerChunk::generate(int level)
{
    const auto paddedVolume = addPaddingToVolume({ _volume.dimensions().x, _volume.dimensions().y, 0 });

    switch (level)
    {
    case 1:
        return assignBiomesToVoronoiCells(paddedVolume);
    case 2:
        return generateBiomeSlots(paddedVolume);
    default:
        break;
    }

    return GenerationState::Complete;
}

void BiomeLayerChunk::assignBiomeToVoronoiCell(sf::Vector3i centroid, BiomeCentroids& biomeCentroids, const ClimateValues& climateValues)
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

    if (potentialBiomes.size() == 1)
    {
        biomeCentroids.emplace(spatial::toXY(centroid), potentialBiomes.front());
    }
    else if (potentialBiomes.size() > 1)
    {
        drft::rng::Random random{ getGlobalSeed() + std::hash<sf::Vector3i>()(centroid)};
        size_t index = random.intInRange(0, potentialBiomes.size());
        biomeCentroids.emplace(spatial::toXY(centroid), potentialBiomes.at(index));
    }
    else if (!rankings.empty())
    {
        biomeCentroids.emplace(spatial::toXY(centroid), rankings.begin()->second);
    }
}

std::unordered_map<entt::id_type, float> BiomeLayerChunk::getClimateValuesAtPoint(sf::Vector3i point, const std::unordered_map<entt::id_type, IGetValueAtLayer*>& generatedDependencies) const
{
    std::unordered_map<entt::id_type, float> result;
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

    std::unordered_map<entt::id_type, IGetValueAtLayer*> generatedDependencies;
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
        assignBiomeToVoronoiCell(point, biomePoints, values);
    }

    biomePositions = util::extractKeys(biomePoints);

    return GenerationState::Complete;
}

GenerationState BiomeLayerChunk::generateBiomeSlots(spatial::AABB<int> volume)
{
    std::unordered_map<entt::id_type, IGetValueAtLayer*> dependencies;
    for (auto&& [_, biome] : biomePoints)
    {
        for (auto dependency : biome->getEntitySlotDependencyIds())
        {
            auto dep = generateDependency<IGetValueAtLayer>(dependency, volume);
            if (!dep.isReady()) return dep.getState();

            dependencies.emplace(dependency, &dep.unwrap());
        }
    }

    drft::spatial::forEachPointInRect(_volume.flatten(), [this, &dependencies, z = _volume.min.z](sf::Vector2i point)
        {
            auto closestPoint = drft::spatial::findClosestPoint2d(point, biomePositions);
            if (!biomePoints.contains(closestPoint)) return;

            const Biome* biome = biomePoints.at(closestPoint);
            if (!biome) return;

            std::unordered_map<entt::id_type, float> layerValues;
            for (auto&& [id, layer] : dependencies)
            {
                layerValues.emplace(id, layer->getValueAt({ point.x, point.y, z }));
            }

            auto slots = biome->determineValidEntitySlots(layerValues);
            for (auto&& slot : slots)
            {
                biomeSlotPoints.emplace_back(biome, slot, point);
            }
        });

    return GenerationState::Complete;
}

BiomeLayer::BiomeLayer(const BiomeRegistry& biomeRegistry)
    : GenerationLayer({8, 8, 8})
    , _biomeRegistry(biomeRegistry)
{
   
}

void drft::BiomeLayer::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("biomes"))
    {
        for (auto&& val : json["biomes"].GetArray())
        {
            std::string biomeName = val.GetString();
            const Biome* biome = _biomeRegistry.get(biomeName);
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

std::vector<BiomeSlotPoint> BiomeLayer::getBiomeEntitySlotPointsInArea(sf::IntRect area, sf::Vector3i origin)
{
    std::vector<BiomeSlotPoint> result;
    forEachLoadedChunkInArea(area, origin, [&area, &result](BiomeLayerChunk& chunk)
        {
            for (auto&& bsp : chunk.biomeSlotPoints)
            {
                if (!area.contains(bsp.point)) continue;
                result.emplace_back(bsp);
            }
        });
    return result;
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





