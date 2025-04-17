#pragma once
#include <unordered_map>
#include <EnTT/entt.h>
#include <type_traits>

namespace drft
{
	class CanvasLayer;
}

struct GenerationRegistries;

struct GenerationContext
{
	unsigned int seed = 0;
	std::unordered_map<entt::id_type, std::reference_wrapper<const drft::CanvasLayer>> layers;
	const GenerationRegistries& registries;
};