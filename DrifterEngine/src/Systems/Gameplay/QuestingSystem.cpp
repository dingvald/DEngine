#include "pch.h"
#include "QuestingSystem.h"
#include "Components/Components.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Spatial/WorldGrid.h"
#include "Systems/Helpers/AddToContainer.h"
#include "ProcGen/WorldGenerator.h"
#include "Random/RandomNumberGenerator.h"


static constexpr int MAXIMUM_GOAL_DISTANCE = 18;
static constexpr int MINIMUM_GOAL_DISTANCE = 15;

void drft::system::QuestingSystem::init()
{

}

void drft::system::QuestingSystem::onStart(bool isNewGame)
{
	if (isNewGame)
	{
		
	}
}

void drft::system::QuestingSystem::fixedUpdate()
{

}

void drft::system::QuestingSystem::save(cereal::JSONOutputArchive& oarchive)
{

}

void drft::system::QuestingSystem::load(cereal::JSONInputArchive& iarchive)
{

}
