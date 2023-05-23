#include "pch.h"
#include "HorrorSpawningSystem.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Random/RandomNumberGenerator.h"
#include "Spatial/Helpers.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Factory/EntityFactory.h"

static constexpr int CHANCE_TO_SPAWN_HORROR = 10;

void drft::system::HorrorSpawningSystem::init()
{
	auto& dispatcher = registry->ctx().get<entt::dispatcher&>();
	dispatcher.sink<events::DayStartEvent>().connect<&HorrorSpawningSystem::onDayStartEvent>(this);
	dispatcher.sink<events::NightStartEvent>().connect<&HorrorSpawningSystem::onNightStartEvent>(this);
	dispatcher.sink<events::GameTickEvent>().connect<&HorrorSpawningSystem::onGameTickEvent>(this);
}

void drft::system::HorrorSpawningSystem::update(const float dt)
{
	if (!_isNight)
	{
		auto horrorView = registry->view<component::Horror>();
		for (auto entity : horrorView)
		{
			registry->emplace_or_replace<component::action::Die>(entity);
		}
	}
}

void drft::system::HorrorSpawningSystem::onDayStartEvent(events::DayStartEvent& ev)
{
	_isNight = false;
}

void drft::system::HorrorSpawningSystem::onNightStartEvent(events::NightStartEvent& ev)
{
	_isNight = true;
}

void drft::system::HorrorSpawningSystem::onGameTickEvent(events::GameTickEvent& ev)
{
	if (_isNight)
	{
		auto roll = rng::RandomNumberGenerator::intInRange(0, 100);
		if (roll <= CHANCE_TO_SPAWN_HORROR)
		{
			spawnHorror();
		}
	}
}

void drft::system::HorrorSpawningSystem::spawnHorror() const
{
	std::cout << "SPAWING A HORROR OHH NOOOO!" << std::endl;

	auto playerView = registry->view<component::Player, component::Position>();
	for (auto [entity, player, pos] : playerView.each())
	{
		const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
		
		auto position = rng::RandomNumberGenerator::positionInCircle(spatial::toTileSpace(pos.position), 96);
		auto isBlocking = [this](entt::entity entity) -> bool
		{
			if (auto physical = registry->try_get<component::Physical>(entity))
			{
				if (physical->blocks) return true;
			}
			return false;
		};

		auto blockers = grid.entitiesAt(position, isBlocking);
		while (!blockers.empty() && spatial::distance(position, spatial::toTileSpace(pos.position)) < 16)
		{
			position = rng::RandomNumberGenerator::positionInCircle(spatial::toTileSpace(pos.position), 96);
			blockers = grid.entitiesAt(position, isBlocking);
		}

		const auto& factory = registry->ctx().get<const EntityFactory&>();
		auto choice = rng::RandomNumberGenerator::intInRange(0, 2);
		entt::handle horror = {};
		switch (choice)
		{
		case 0:
			horror = factory.build("Birdman Horror", *registry);
			break;
		case 1:
			horror = factory.build("Skeletal Horror", *registry);
			break;
		case 2:
			horror = factory.build("Unknown Horror", *registry);
			break;
		default:
			horror = factory.build("Unknown Horror", *registry);
			break;
		}
		horror.emplace<component::Horror>(true);
		horror.patch<component::Position>([position](component::Position& pos)
			{
				pos.position = spatial::toWorldSpace(position);
			});
	}
}
