#pragma once
#include <unordered_map>
#include <EnTT/entt.h>

class CanvasLayer;
struct GenerationRegistries;

struct GenerationContext
{
	unsigned int seed = 0;
	std::unordered_map<entt::id_type, std::reference_wrapper<const CanvasLayer>> layers;
	const GenerationRegistries& registries;
};