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
	
}
