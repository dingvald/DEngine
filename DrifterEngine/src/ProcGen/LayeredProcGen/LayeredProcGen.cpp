#include "pch.h"
#include "LayeredProcGen.h"

namespace drft
{
    GenerationLayerManager::GenerationLayerManager(GenerationLayerManager&& other)
        : _layers(std::move(other._layers))
        , _canvasLayers(std::move(other._canvasLayers))
        , _layerFactory(std::move(other._layerFactory))
        , _globalSeed(other._globalSeed)
        , _generationRegistries(other._generationRegistries)
        , _entityPack(other._entityPack)
    {
        // No additional logic required
    }

    GenerationLayerManager& GenerationLayerManager::operator=(GenerationLayerManager&& other)
    {
        if (this != &other)
        {
            _layers = std::move(other._layers);
            _canvasLayers = std::move(other._canvasLayers);
            _layerFactory = std::move(other._layerFactory);
            _globalSeed = other._globalSeed;
            // _generationRegistries is a reference, assumed to be valid and not reassigned
            _entityPack = other._entityPack;
        }
        return *this;
    }
}