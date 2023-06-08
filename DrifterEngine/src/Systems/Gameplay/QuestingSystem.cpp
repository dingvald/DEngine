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
		setNewGoalPosition({0,0});

		const auto& factory = registry->ctx().get<const EntityFactory&>();
		auto player = registry->view<component::Player>();

		auto map = factory.build("Ephemera", *registry);
		map.patch<component::Info>([](component::Info& info) 
			{
				info.name = "Strange Map";
				info.description = "X marks the spot... but what spot?";
			});
		map.patch<component::Usable>([this](component::Usable& usable)
			{
				usable.action = "Read";
				usable.params["Type"] = "map"; 
				usable.params["X"] = std::to_string(_goalPosition.x);
				usable.params["Y"] = std::to_string(_goalPosition.y);
			});

		addToContainer(*registry, player.front(), map.entity());
	}
}

void drft::system::QuestingSystem::fixedUpdate()
{
	auto playerView = registry->view<component::Player, component::Position>();
	for (auto [entity, player, pos] : playerView.each())
	{
		if (!_isGoalBuilt 
			&& spatial::distance(spatial::toChunkCoordinate(pos.position), _goalPosition) < 2)
		{
			buildGoalEntity();
		}
	}
}

void drft::system::QuestingSystem::save(cereal::JSONOutputArchive& oarchive)
{
	oarchive(cereal::make_nvp("Goal Position X", _goalPosition.x));
	oarchive(cereal::make_nvp("Goal Position Y", _goalPosition.y));
}

void drft::system::QuestingSystem::load(cereal::JSONInputArchive& iarchive)
{
	iarchive(cereal::make_nvp("Goal Position X", _goalPosition.x));
	iarchive(cereal::make_nvp("Goal Position Y", _goalPosition.y));
}

void drft::system::QuestingSystem::setNewGoalPosition(sf::Vector2i currentCoordinate)
{
	_isGoalBuilt = false;
	const auto& generator = registry->ctx().get<const gen::WorldGenerator&>();
	_goalPosition = rng::RandomNumberGenerator::positionInCircle(currentCoordinate, MAXIMUM_GOAL_DISTANCE);
	while (spatial::distance(currentCoordinate, _goalPosition) < MINIMUM_GOAL_DISTANCE
		|| generator.getBiomeType(_goalPosition) == gen::BiomeType::Lake)
	{
		_goalPosition = rng::RandomNumberGenerator::positionInCircle(currentCoordinate, MAXIMUM_GOAL_DISTANCE);
	}
}

void drft::system::QuestingSystem::buildGoalEntity()
{
	const auto& factory = registry->ctx().get<const EntityFactory&>();
	const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
	auto key = factory.build("Dungeon Key", *registry);
	
	sf::Vector2i localPosition;
	localPosition.x = rng::RandomNumberGenerator::intInRange(0, spatial::CHUNK_WIDTH);
	localPosition.y = rng::RandomNumberGenerator::intInRange(0, spatial::CHUNK_HEIGHT);
	sf::Vector2i tilePosition = spatial::toTileSpace(_goalPosition) + localPosition;
	auto entities = grid.entitiesAt(tilePosition,
		[this](auto entity) -> bool
		{
			return registry->any_of<component::Physical>(entity);
		});

	while (!entities.empty())
	{
		localPosition.x = rng::RandomNumberGenerator::intInRange(0, spatial::CHUNK_WIDTH);
		localPosition.y = rng::RandomNumberGenerator::intInRange(0, spatial::CHUNK_HEIGHT);
		tilePosition = spatial::toTileSpace(_goalPosition) + localPosition;
		entities = grid.entitiesAt(tilePosition,
			[this](auto entity) -> bool
			{
				return registry->any_of<component::Physical>(entity);
			});
	}
	
	key.patch<component::Position>([tilePosition](component::Position& pos)
		{
			pos.position = spatial::toWorldSpace(tilePosition);
		});

	_isGoalBuilt = true;
}
