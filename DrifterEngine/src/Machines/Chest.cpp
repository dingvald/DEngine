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
	
}
