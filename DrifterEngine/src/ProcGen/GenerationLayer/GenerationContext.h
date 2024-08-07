#pragma once
#include <EnTT/container/dense_map.hpp>

class GenerationLayerManager;

struct GenerationContext
{
	sf::IntRect area;
	GenerationLayerManager& layers;
};