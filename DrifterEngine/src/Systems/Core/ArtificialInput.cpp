#include "pch.h"
#include "ArtificialInput.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Random/RandomNumberGenerator.h"
#include "Spatial/WorldGrid.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "Systems/Gameplay/FactionSystem.h"


void drft::system::ArtificialInput::init()
{
}

void drft::system::ArtificialInput::update(const float dt)
{
	auto view = registry->view<component::AI, const component::Position, component::tag::CurrentActor>();
	for (auto [entity, ai, myPos] : view.each())
	{
		const auto target = registry->try_get<component::Position>(ai.target);
		if (target)
		{
			const auto myTilePosition = spatial::toTileSpace(myPos.position);
			const auto targetTilePosition = spatial::toTileSpace(target->position);

			if (spatial::distance(myTilePosition, targetTilePosition) <= ai.sightRange)
			{
				if (hasLineOfSight(myTilePosition, targetTilePosition))
				{
					clearPathCache(entity);
					moveToTarget(entity, myTilePosition, targetTilePosition);
				}
				else
				{
					pathToTarget(entity, myTilePosition, targetTilePosition);
				}
			}
			else
			{
				clearPathCache(entity);
				ai.target = entt::null;
			}
		}
		else
		{
			ai.target = findTarget({ *registry, entity });
			randomMove({ *registry, entity });
		}
	}
}

entt::entity drft::system::ArtificialInput::findTarget(entt::handle entity) const
{
	if (!entity.all_of<component::Faction>())
	{
		return entt::null;
	}

	const auto& faction = entity.get<component::Faction>();
	const auto& ai = entity.get<component::AI>();
	const auto& pos = entity.get<component::Position>();

	float closestRange = 1000;
	entt::entity closestTarget = entt::null;

	auto factionView = registry->view<const component::Faction, const component::Position, component::tag::Active>();
	for (auto [otherEnt, otherfaction, otherPos] : factionView.each())
	{
		if (FactionSystem::resolveRelationship(faction.name, otherfaction.name) == Relationship::Hostile)
		{
			const float distance = spatial::distance(spatial::toTileSpace(pos.position), spatial::toTileSpace(otherPos.position));
			if (distance < ai.sightRange && distance < closestRange)
			{
				if (hasLineOfSight(spatial::toTileSpace(pos.position), spatial::toTileSpace(otherPos.position)))
				{
					closestRange = distance;
					closestTarget = otherEnt;
				}
			}
		}
	}
	
	return closestTarget;
}

bool drft::system::ArtificialInput::hasLineOfSight(sf::Vector2i myPosition, sf::Vector2i targetPosition) const
{
	auto tilesInLOS = spatial::getIntPointsAlongLine(myPosition, targetPosition);
	const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
	for (auto tile : tilesInLOS)
	{
		auto entities = grid.entitiesAt(tile,
			[this](entt::entity entity) -> bool
			{
				return registry->any_of<component::LightBlocking>(entity);
			});

		if (!entities.empty()) return false;
	}

	return true;
}

void drft::system::ArtificialInput::randomMove(entt::handle entity) const
{
	int randx = rng::RandomNumberGenerator::intInRange(-1, 1);
	int randy = rng::RandomNumberGenerator::intInRange(-1, 1);
	const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
	const auto& tilepos = spatial::toTileSpace(entity.get<component::Position>().position);

	auto blockerFilter = [this](entt::entity entity) -> bool
	{
		if (auto physical = registry->try_get<component::Physical>(entity))
		{
			return physical->blocks;
		}
		return false;
	};

	auto blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);
	
	int safetyCount = 0; // in case entity is surrounded
	while (safetyCount < 8 && !blockers.empty())
	{
		randx = rng::RandomNumberGenerator::intInRange(-1, 1);
		randy = rng::RandomNumberGenerator::intInRange(-1, 1);
		blockers = grid.entitiesAt(tilepos + sf::Vector2i(randx, randy), blockerFilter);
		++safetyCount;
	}
	
	entity.emplace<component::action::Move>(sf::Vector2i(randx, randy));
}

void drft::system::ArtificialInput::moveToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const
{
	auto line = spatial::getIntPointsAlongLine(myPosition, targetPosition);
	sf::Vector2i delta;
	if (line.empty())
	{
		delta = myPosition - targetPosition;
	}
	else
	{
		delta = myPosition - line.front();
	}
	int xMove = delta.x == 0 ? 0 : -(delta.x / abs(delta.x));
	int yMove = delta.y == 0 ? 0 : -(delta.y / abs(delta.y));

	registry->emplace<component::action::Move>(ai, sf::Vector2i{ xMove, yMove });
}

void drft::system::ArtificialInput::pathToTarget(entt::entity ai, sf::Vector2i myPosition, sf::Vector2i targetPosition) const
{
	if (!_cachedPaths.contains(ai) || _cachedPaths.at(ai).empty())
	{
		const auto& grid = registry->ctx().get<const spatial::WorldGrid&>();
		_cachedPaths[ai] = grid.getPath(myPosition, targetPosition, 
			[this](const std::vector<entt::entity>& entities) -> int
			{
				for (auto entity : entities)
				{
					if (auto physical = registry->try_get<component::Physical>(entity))
					{
						if (physical->blocks) return 10000;
					}
				}
				return 0;
			});
	}
	if (_cachedPaths.at(ai).empty())
	{
		moveToTarget(ai, myPosition, targetPosition);
	}
	else
	{
		moveToTarget(ai, myPosition, _cachedPaths.at(ai).front());
		_cachedPaths.at(ai).pop_front();
	}
}

void drft::system::ArtificialInput::clearPathCache(entt::entity entity) const
{
	if (!_cachedPaths.contains(entity)) return;
	_cachedPaths.erase(entity);
}
