#include "pch.h"
#include "BiomeLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

#include <Random/Random.h>
#include <Spatial/Helpers.h>
#include <Utility/ContainerHelpers.h>
#include <Utility/stdHashing.h>
#include <JSON/StringExpressions.h>

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
    auto& biomes = _layer.getBiomeRegistry();
    std::vector<const Biome*> potentialBiomes;
    biomes.forEachBiome([&climateValues, &potentialBiomes](const std::string& name, const Biome& biome)
        {
            if (!biome.satisfiesClimate(climateValues)) return;
            potentialBiomes.push_back(&biome);
        });

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
}

std::unordered_map<entt::id_type, float> BiomeLayerChunk::getClimateValuesAtPoint(sf::Vector3i point, const std::unordered_map<entt::id_type, OnDemandLayer*>& generatedDependencies) const
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

    std::unordered_map<entt::id_type, OnDemandLayer*> climateDependencies;
    for (auto&& dependencyID : _layer.getClimateDependencies())
    {
        auto depLayer = generateDependency<OnDemandLayer>(dependencyID, volume);
        if (!depLayer.isReady()) return depLayer.getState();

        climateDependencies.emplace(dependencyID, &depLayer.unwrap());
    }

    const auto centroids = voronoiLayer.unwrap().getCentroidsInArea(volume.flatten(), volume.center());
    for (auto&& point : centroids)
    {
        const auto values = getClimateValuesAtPoint(point, climateDependencies);
        assignBiomeToVoronoiCell(point, biomePoints, values);
    }

    biomePositions = util::extractKeys(biomePoints);

    return GenerationState::Complete;
}

GenerationState BiomeLayerChunk::generateBiomeSlots(spatial::AABB<int> volume)
{
    std::unordered_map<entt::id_type, OnDemandLayer*> dependencies;
    for (auto&& [_, biome] : biomePoints)
    {
        for (auto&& [slotID, slotDeterminer] : biome->getSlotDeterminers())
        {
            for (auto&& [layerID, _] : slotDeterminer.dependencies)
            {
                auto dep = generateDependency<OnDemandLayer>(layerID, volume);
                if (!dep.isReady()) return dep.getState();

                dependencies.emplace(layerID, &dep.unwrap());
            }
        }
    }

    drft::spatial::forEachPointInRect(_volume.flatten(), [this, &dependencies, z = _volume.min.z](sf::Vector2i point)
        {
            auto closestPoint = drft::spatial::findClosestPoint2d(point, biomePositions);
            const Biome* biome = biomePoints.at(closestPoint);

            for (auto&& [slotID, slotDeterminer] : biome->getSlotDeterminers())
            {
                TokenValues values;
                for (auto&& [layerID, slotDependency] : slotDeterminer.dependencies)
                {
                    auto layer = dependencies.at(layerID);
                    float val = layer->getValueAt({point.x, point.y, z});
                    values.emplace(layerID, slotDependency.satisfiesValue(val, slotDependency.range));
                }
                if (slotDeterminer.expression.evaluate(values))
                {
                    biomeSlotPoints.emplace_back( biome, slotID, point );
                }
            }
        });

    return GenerationState::Complete;
}

BiomeLayer::BiomeLayer()
    : GenerationLayer({8, 8, 8})
{}

void drft::BiomeLayer::createFromJson(const rapidjson::Value& json)
{
    _biomes.createFromJson(json);
    _biomes.forEachBiome([this](const std::string& name, const Biome& biome)
        {
            for (auto&& [id, _] : biome.getClimateRanges())
            {
                _climateDependencies.insert(id);
            }
        });
}

const BiomeRegistry& BiomeLayer::getBiomeRegistry() const
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





