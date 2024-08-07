#include "pch.h"
#include "GenerationLayerManager.h"

GenerationState GenerationLayerManager::generate(entt::id_type id, sf::IntRect area)
{
	if (_layers.contains(id))
	{
		return _layers[id]->generate({ .area = area, .layers = *this });
	}
	return GenerationState::Failed;
}

AbstractLayer& GenerationLayerManager::get(entt::id_type id)
{
	return *_layers[id];
}
