#include "pch.h"
#include "Chest.h"
#include "Factory/EntityFactory.h"
#include "Components/Components.h"
#include "ProcGen/Biome.h"

static constexpr int MACHINE_WIDTH = 3;
static constexpr int MACHINE_HEIGHT = 3;

sf::Vector2i drft::machine::Chest::getDimensions() const
{
	return { MACHINE_WIDTH, MACHINE_HEIGHT };
}

void drft::machine::Chest::layout()
{
	// Create and place chest
	entt::handle chest = _factory->build("Chest", *_registry);
	setRelativePosition(chest, { 1,1 });

	// Surround chest with the biome's base environmental entity
	for (int y = 0; y < MACHINE_HEIGHT; ++y)
	{
		for (int x = 0; x < MACHINE_WIDTH; ++x)
		{
			if (x == 1 && y == 1) continue;
			auto pickedEntity = _biome->pickRandomEnvironmental("Base");
			entt::handle baseEnv = _factory->build(pickedEntity, *_registry);
			setRelativePosition(baseEnv, { x,y });
		}
	}
}
