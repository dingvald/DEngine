#include "pch.h"
#include "WildernessHorde.h"
#include "Factory/EntityFactory.h"
#include "Components/Components.h"
#include "ProcGen/Biome.h"

static constexpr int MACHINE_WIDTH = 9;
static constexpr int MACHINE_HEIGHT = 9;

sf::Vector2i drft::machine::WildernessHorde::getDimensions() const
{
	return { MACHINE_WIDTH, MACHINE_HEIGHT };
}

void drft::machine::WildernessHorde::layout()
{
	std::vector<std::pair<std::string, float>> params;
	for (auto& [name, value] : _biome->machines().at("Wilderness Horde").params)
	{
		auto pair = std::make_pair(name, static_cast<float>(std::atof(value.c_str())));
		params.push_back(pair);
	}

	auto pickedName = _biome->pickRandomWildlife(params);
	if (!pickedName.has_value()) return;
	for (int y = 0; y < MACHINE_HEIGHT; ++y)
	{
		for (int x = 0; x < MACHINE_WIDTH; ++x)
		{
			if (x % 2) continue;
			if ((y + 1) % 2) continue;
			entt::handle baseEnv = _factory->build(pickedName.value(), *_registry);
			setRelativePosition(baseEnv, { x,y });
		}
	}
}
