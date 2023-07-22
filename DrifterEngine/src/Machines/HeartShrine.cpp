#include "pch.h"
#include "HeartShrine.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Helpers.h"
#include "Components/Components.h"
#include "ProcGen/Biome.h"

static constexpr int MACHINE_WIDTH = 10;
static constexpr int MACHINE_HEIGHT = MACHINE_WIDTH;

sf::Vector2i drft::machine::HeartShrine::getDimensions() const
{
	return { MACHINE_WIDTH, MACHINE_HEIGHT };
}

void drft::machine::HeartShrine::layout()
{
	sf::Vector2i centerPosition = { MACHINE_WIDTH / 2, MACHINE_HEIGHT / 2 };
	auto positionsInCircle = spatial::getIntCircleInRadius(centerPosition, MACHINE_WIDTH / 2);
	for (auto position : positionsInCircle)
	{
		if (position.x == centerPosition.x && (position.y > centerPosition.y))
		{
			auto grass = _factory->build("Grass", *_registry);
			setRelativePosition(grass, position);
			continue;
		}
		if (position == centerPosition)
		{
			auto grass = _factory->build("Grass", *_registry);
			setRelativePosition(grass, position);
			auto heartShrine = _factory->build("Heart Shrine", *_registry);
			setRelativePosition(heartShrine, position);
			continue;
		}
		auto water = _factory->build("Water", *_registry);
		setRelativePosition(water, position);
	}
}
