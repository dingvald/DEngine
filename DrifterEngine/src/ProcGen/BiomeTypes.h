#pragma once

namespace drft::gen
{
	enum class BiomeType
	{
		Forest, // Trees, stones, grass, vegetation, fungi
		Mountain, // stones, boulders, trees, minerals, 
		Lake, // Water, grass, 
		Swamp, // Water, mud, trees, stones
		Desert, // Not much
		Grassland // Grass
	};

	const std::unordered_map<BiomeType, std::string_view> Biome2String =
	{
		{BiomeType::Forest, "Forest"},
		{BiomeType::Mountain, "Mountain"},
		{BiomeType::Lake, "Lake"},
		{BiomeType::Swamp, "Swamp"},
		{BiomeType::Desert, "Desert"},
		{BiomeType::Grassland, "Grassland"}
	};

	const std::unordered_map<std::string_view, BiomeType> String2Biome =
	{
		{"Forest", BiomeType::Forest},
		{"Mountain", BiomeType::Mountain},
		{"Lake", BiomeType::Lake},
		{"Swamp", BiomeType::Swamp},
		{"Desert", BiomeType::Desert},
		{"Grassland", BiomeType::Grassland}
	};

	struct BiomePair
	{
		BiomeType main;
		BiomeType secondary;
	};
}