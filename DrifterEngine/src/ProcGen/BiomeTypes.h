#pragma once
#include "Utility/SpriteIndexer.h"

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

	const std::unordered_map<BiomeType, util::Sprite> BiomeSprites =
	{
		{BiomeType::Forest, util::Sprite::Tree},
		{BiomeType::Mountain, util::Sprite::Triangle},
		{BiomeType::Lake, util::Sprite::Water},
		{BiomeType::Swamp, util::Sprite::Water},
		{BiomeType::Desert, util::Sprite::Sand},
		{BiomeType::Grassland, util::Sprite::Grass},
	};

	const std::unordered_map<BiomeType, sf::Color> BiomeColors =
	{
		{BiomeType::Forest, sf::Color(31, 51, 36)},
		{BiomeType::Mountain, sf::Color(80,80,80)},
		{BiomeType::Lake, sf::Color(27, 149, 224)},
		{BiomeType::Swamp, sf::Color(30,80,100)},
		{BiomeType::Desert, sf::Color(194, 178, 128)},
		{BiomeType::Grassland, sf::Color(72,111,56)},
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