#include "pch.h"
#include "OreDeposit.h"
#include "ProcGen/Biome.h"
#include "Factory/EntityFactory.h"
#include "Random/RandomNumberGenerator.h"
#include "ProcGen/PlacementAlgorithms/OrganicSpread.h"

sf::Vector2i drft::machine::OreDeposit::getDimensions() const
{
    return { 10,10 };
}

void drft::machine::OreDeposit::layout()
{
	std::vector<std::pair<std::string, float>> params;
	for (auto& [name, value] : _biome->machines().at("Ore Deposit").params)
	{
		auto pair = std::make_pair(name, static_cast<float>(std::atof(value.c_str())));
		params.push_back(pair);
	}
	float sum = 0.f;
	for (auto&& [cat, weight] : params)
	{
		sum += weight;
	}
	float choice = rng::RandomNumberGenerator::realInRange(0, sum);
	auto rng = std::default_random_engine{};
	std::shuffle(params.begin(), params.end(), rng);
	std::string depositType = "";
	for (auto&& [type, weight] : params)
	{
		sum -= weight;
		if (choice > sum)
		{
			depositType = type;
			break;
		}
	}
	const gen::GenerationParameters genParams =
	{
		{"Generations", 2.0},
		{"StartingSeeds", 1.0},
		{"SeedsPerGeneration", 2.0},
		{"Radius", 2.0}
	};
	auto positions = gen::organicSpread(0, spatial::Grid<gen::CellState>{getDimensions().x, getDimensions().y}, genParams);
	for (auto position : positions)
	{
		auto entity = _factory->build(depositType, *_registry);
		setRelativePosition(entity, position);
	}
}
