#pragma once
#include <unordered_map>
#include <EnTT/entt.h>

class CanvasLayer;
struct GenerationRegistries;

struct GenerationContext
{
	unsigned int seed = 0;
	const GenerationRegistries& registries;
};