#include "pch.h"
#include "BiomeLayer.h"
#include <ProcGen/Layers/VoronoiLayer.h>

#include <Random/Random.h>
#include <Spatial/Helpers.h>
#include <Utility/stdHashing.h>


using namespace entt::literals;
using namespace drft;


GenerationState BiomeLayerChunk::generate(GenerationLevel desiredLevel)
{
    const auto paddedVolume = addPaddingToVolume({ _volume.dimensions().x, _volume.dimensions().y, 0 });

    switch (desiredLevel)
    {
    case GenerationLevel::One:
        return assignBiomesToVoronoiCells(paddedVolume);
    case GenerationLevel::Two:
        return generateBiomeFeatures(paddedVolume);
    case GenerationLevel::Three:
        return generateBiomeSlots(paddedVolume);
    default:
        break;
    }

    return GenerationState::Complete;
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

    auto& canvas = _layer.getLayerManager().getCanvas("something"_hs);

    

    const auto centroids = voronoiLayer.unwrap().getCentroidsInArea(volume.flatten(), volume.center());
    for (auto&& point : centroids)
    {

        const auto values = getClimateValuesAtPoint(point, generatedDependencies);
        assignBiomeToVoronoiCell(point, values);
    }

    return GenerationState::Complete;
}

GenerationState drft::BiomeLayerChunk::generateBiomeFeatures(spatial::AABB<int> volume)
{
    auto centerPoint = volume.flatten().getCenter();
    const auto closestBiomePosition = drft::spatial::findClosestPoint2d(centerPoint, biomePositions);

    const Biome* biome = biomePoints.at(closestBiomePosition);
    if (!biome) return GenerationState::Failed;
    

    return GenerationState::Complete;
}

GenerationState BiomeLayerChunk::generateBiomeSlots(spatial::AABB<int> volume)
{
    GeneratedDependencies dependencies;
    for (auto&& [_, biome] : biomePoints)
    {
        for (auto dependency : biome->getEntitySlotDependencyIds())
        {
            auto dep = generateDependency<IGetValueAtLayer>(dependency, volume);
            if (!dep.isReady()) return dep.getState();

            dependencies.emplace(dependency, &dep.unwrap());
        }
    }

    // Individual entities
    drft::spatial::forEachPointInRect(_volume.flatten(), [this, &dependencies, z = _volume.min.z](sf::Vector2i point)
        {
            auto closestPoint = drft::spatial::findClosestPoint2d(point, biomePositions);
            if (!biomePoints.contains(closestPoint)) return;

            const Biome* biome = biomePoints.at(closestPoint);
            if (!biome) return;

            Biome::DependencyValues layerValues;
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

sf::Vector3i drft::BiomeLayer::getChunkDimensions() const
{
    return { 8, 8, 8 };
}
