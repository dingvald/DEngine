#include "pch.h"
#include "HeartShrine.h"
#include "Factory/EntityFactory.h"
#include "Components/Components.h"
#include "ProcGen/Biome.h"

static constexpr int MACHINE_WIDTH = 5;
static constexpr int MACHINE_HEIGHT = 5;

sf::Vector2i drft::machine::HeartShrine::getDimensions() const
{
	return { MACHINE_WIDTH, MACHINE_HEIGHT };
}

void drft::machine::HeartShrine::layout()
{
	for (int y = 0; y < MACHINE_HEIGHT; ++y)
	{
		for (int x = 0; x < MACHINE_WIDTH; ++x)
		{
			if (x == 2 && (y == 3 || y == 4))
			{
				auto grass = _factory->build("Grass", *_registry);
				setRelativePosition(grass, { x,y });
				continue;
			}
			if (x == 2 && y == 2)
			{
				auto grass = _factory->build("Grass", *_registry);
				setRelativePosition(grass, { x,y });
				auto heartShrine = _factory->build("Heart Shrine", *_registry);
				setRelativePosition(heartShrine, { x,y });
				continue;
			}
			auto water = _factory->build("Water", *_registry);
			setRelativePosition(water, { x,y });
		}
	}
}
