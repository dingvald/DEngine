#include "pch.h"
#include "BiomeLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

#include <Random/Random.h>
#include <Spatial/Helpers.h>
#include <Utility/ContainerHelpers.h>
#include <Utility/stdHashing.h>
#include <JSON/StringExpressions.h>

using namespace entt::literals;

static const std::filesystem::path BIOME_FOLDER_PATH = "./data/static/biomes";

GenerationState BiomeLayerChunk::generate(int level)
{
    const auto paddedBounds = addPaddingToBounds({ _bounds.width, _bounds.height });

    switch (level)
    {
    case 1:
        return assignBiomesToVoronoiCells(paddedBounds);
    case 2:
        return generateBiomeSlots(paddedBounds);
    default:
        break;
    }

    return GenerationState::Complete;
}

void BiomeLayerChunk::assignBiomeToVoronoiCell(sf::Vector2i centroid, BiomeCentroids& biomeCentroids, const ClimateValues& climateValues)
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
        biomeCentroids.emplace(centroid, potentialBiomes.front());
    }
    else if (potentialBiomes.size() > 1)
    {
        drft::rng::Random random{ getGlobalSeed() + std::hash<sf::Vector2i>()(centroid)};
        size_t index = random.intInRange(0, potentialBiomes.size());
        biomeCentroids.emplace(centroid, potentialBiomes.at(index));
    }
}

std::unordered_map<std::string, float> BiomeLayerChunk::getClimateValuesAtPoint(sf::Vector2i point, const std::unordered_map<std::string, IGetValueAt*>& generatedDependencies) const
{
    std::unordered_map<std::string, float> result;
    for (auto&& [name, layerPtr] : generatedDependencies)
    {
        if (!layerPtr) continue; // TODO: Should maybe be an assert...?
        result.emplace(name, layerPtr->getValueAt(point));
    }
    return result;
}

GenerationState BiomeLayerChunk::assignBiomesToVoronoiCells(sf::IntRect area)
{
    auto voronoiLayer = generateDependency<VoronoiLayer>(area);
    if (!voronoiLayer.isReady()) return voronoiLayer.getState();

    std::unordered_map<std::string, IGetValueAt*> climateDependencies;
    for (auto&& depName : _layer.getClimateDependencies())
    {
        auto depLayer = generateDependency<IGetValueAt>(entt::hashed_string{ depName.c_str() }, _bounds);
        if (!depLayer.isReady()) return depLayer.getState();

        climateDependencies.emplace(depName, &depLayer.unwrap());
    }

    for (sf::Vector2i point : voronoiLayer.unwrap().getCentroidsInBounds(area))
    {
        const auto values = getClimateValuesAtPoint(point, climateDependencies);
        assignBiomeToVoronoiCell(point, biomePoints, values);
    }

    return GenerationState::Complete;
}

GenerationState BiomeLayerChunk::generateBiomeSlots(sf::IntRect area)
{
    std::unordered_map<entt::id_type, IGetValueAt*> dependencies;
    for (auto&& [point, biome] : biomePoints)
    {
        for (auto&& [slotID, slotDeterminer] : biome->getSlotDeterminers())
        {
            for (auto&& [layerID, _] : slotDeterminer.dependencies)
            {
                auto dep = generateDependency<IGetValueAt>(layerID, area);
                if (!dep.isReady()) return dep.getState();
                dependencies.emplace(layerID, &dep.unwrap());
            }
        }
    }

    auto biomeCentroids = util::extractKeys(biomePoints);

    drft::spatial::forEachPointInRect(_bounds, [this, &biomeCentroids, &dependencies](sf::Vector2i point)
        {
            auto closestPoint = drft::spatial::findClosestPoint(point, biomeCentroids);
            const Biome* biome = biomePoints.at(closestPoint);

            for (auto&& [slotID, slotDeterminer] : biome->getSlotDeterminers())
            {
                TokenValues values;
                for (auto&& [layerID, slotDependency] : slotDeterminer.dependencies)
                {
                    auto layer = dependencies.at(layerID);
                    float val = layer->getValueAt(point);
                    values.emplace(layerID, slotDependency.satisfiesValue(val, slotDependency.range));
                }
                if (slotDeterminer.expression.evaluate(values))
                {
                    biomeSlotPoints.emplace_back(BiomeSlotPoint{ biome, slotID, point });
                }
            }
        });

    return GenerationState::Complete;
}

BiomeLayer::BiomeLayer()
    : GenerationLayer({32, 32})
{
    _biomes.createBiomesFromJSON(BIOME_FOLDER_PATH);
    _biomes.forEachBiome([this](const std::string& name, const Biome& biome)
        {
            for (auto&& [name, _] : biome.getClimateRanges())
            {
                _climateDependencies.insert(name);
            }
        });
}

const BiomeRegistry& BiomeLayer::getBiomeRegistry() const
{
    return _biomes;
}

const std::unordered_set<std::string>& BiomeLayer::getClimateDependencies() const
{
    return _climateDependencies;
}

BiomeCentroids BiomeLayer::getBiomeCentroidsInBounds(sf::IntRect area)
{
    BiomeCentroids result;
    forEachLoadedChunkInArea(area, [&result](BiomeLayerChunk& chunk)
        {
            for (auto&& point : chunk.biomePoints)
            {
                result.emplace(point);
            }
        });
    return result;
}

std::vector<BiomeSlotPoint> BiomeLayer::getBiomeEntitySlotPointsInBounds(sf::IntRect area)
{
    std::vector<BiomeSlotPoint> result;
    forEachLoadedChunkInArea(area, [&area, &result](BiomeLayerChunk& chunk)
        {
            for (auto&& bsp : chunk.biomeSlotPoints)
            {
                if (!area.contains(bsp.point)) continue;
                result.emplace_back(bsp);
            }
        });
    return result;
}





