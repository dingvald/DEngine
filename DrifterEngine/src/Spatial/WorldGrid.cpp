#include "pch.h"
#include "WorldChunk.h"
#include "WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Utility/stdHashing.h"

using namespace drft::spatial;

void drft::spatial::WorldGrid::placeEntity(const entt::entity entity, const sf::Vector2i worldPosition)
{
	if (entity == entt::null) return;
	auto chunkCoordinate = toChunkCoordinate(worldPosition);
	auto localPosition = toLocalChunkSpace(worldPosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair))
	{
		_chunks[keyablePair] = std::make_unique<WorldChunk>(CHUNK_WIDTH, CHUNK_HEIGHT);
	}
	_chunks[keyablePair]->placeEntity(entity, localPosition);
	_entityPositions[entity] = worldPosition;
}

void drft::spatial::WorldGrid::removeEntity(const entt::entity entity)
{
	if (entity == entt::null) return;
	if (!_entityPositions.contains(entity)) return;

	sf::Vector2i worldPosition = getPosition(entity);

	auto chunkCoordinate = toChunkCoordinate(worldPosition);
	auto localPosition = toLocalChunkSpace(worldPosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair)) return;

	_chunks.at(keyablePair)->removeEntity(entity, localPosition);
	_entityPositions.erase(entity);
}

void drft::spatial::WorldGrid::moveEntity(const entt::entity entity, const sf::Vector2i toWorldPosition)
{
	this->removeEntity(entity);
	this->placeEntity(entity, toWorldPosition);
}

sf::Vector2i drft::spatial::WorldGrid::getPosition(const entt::entity entity) const
{
	return _entityPositions.at(entity);
}

EntityList drft::spatial::WorldGrid::entitiesAt(const sf::Vector2i tilePosition) const
{
	auto chunkCoordinate = toChunkCoordinate(tilePosition);
	auto localPosition = toLocalChunkSpace(tilePosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair)) {
		return EntityList{};
	}

	return _chunks.at(keyablePair)->entitiesAt(localPosition);;
}

EntityList drft::spatial::WorldGrid::entitiesAt(const sf::Vector2i tilePosition, std::function<bool(entt::entity)> filterFunc) const
{
	auto chunkCoordinate = toChunkCoordinate(tilePosition);
	auto localPosition = toLocalChunkSpace(tilePosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair)) {
		return EntityList{};
	}
	std::vector<entt::entity> result;
	const auto entities = _chunks.at(keyablePair)->entitiesAt(localPosition);
	for (auto entity : entities)
	{
		if (filterFunc(entity))
		{
			result.emplace_back(entity);
		}
	}

	return result;
}

void drft::spatial::WorldGrid::removeChunk(const sf::Vector2i coordinate)
{
	if (_chunks.contains({ coordinate.x, coordinate.y }))
	{
		_chunks.erase( {coordinate.x, coordinate.y} );
	}
}

EntityList drft::spatial::WorldGrid::getAllEntities(const sf::Vector2i coordinate) const
{
	if (!_chunks.contains({ coordinate.x, coordinate.y }))
	{
		return EntityList{};
	}
	return _chunks.at({ coordinate.x, coordinate.y })->getAllEntities();
}

std::deque<sf::Vector2i> drft::spatial::WorldGrid::getPath(sf::Vector2i pt1, sf::Vector2i pt2, heuristic costFunc) const
{
	// A* 

	struct Node
	{
		sf::Vector2i value;
		int distance = 0;
		int cost = 0;

		bool operator<(const Node& rhs) const
		{
			if (cost == rhs.cost)
			{
				if (value.x == rhs.value.x)
				{
					return value.y < rhs.value.y;
				}
				else
				{
					return value.x < rhs.value.x;
				}
			}
			else
			{
				return cost < rhs.cost;
			}
		}
	};

	std::unordered_set<sf::Vector2i> closedSet;
	std::set<Node> openSet;
	std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;
	auto constructPath = [&cameFrom](sf::Vector2i endPosition) -> std::deque<sf::Vector2i>
	{
		sf::Vector2i currentPosition = endPosition;
		std::deque<sf::Vector2i> path;
		while (currentPosition != cameFrom[currentPosition])
		{
			path.push_front(currentPosition);
			currentPosition = cameFrom[currentPosition];
		}

		return path;
	};
	
	openSet.emplace(Node(pt1,0,0));
	cameFrom[pt1] = pt1;

	const int LIMIT = 50;
	int passes = 0;
	while (!openSet.empty() && passes < LIMIT)
	{
		auto currentNode = *(openSet.begin());
		if (currentNode.value == pt2) return constructPath(currentNode.value);
		openSet.erase(openSet.begin());
		closedSet.emplace(currentNode.value);

		// check neighbors
		for (int y = currentNode.value.y - 1; y <= currentNode.value.y + 1; ++y)
		{
			for (int x = currentNode.value.x - 1; x <= currentNode.value.x + 1; ++x)
			{
				if (closedSet.contains({x,y})) continue;
				cameFrom[{x, y}] = currentNode.value;
				if (sf::Vector2i(x,y) == pt2) return constructPath(sf::Vector2i(x,y));

				int distanceSoFar = currentNode.distance + 1;
				int distanceFromTarget = static_cast<int>(std::sqrtf(std::powf(pt2.x - x, 2.f) + std::powf(pt2.y - y, 2.f)));
				const auto entities = entitiesAt({ x,y });
				int cost = distanceSoFar + distanceFromTarget + costFunc(entities);

				Node neighbor = Node({ x,y }, distanceSoFar, cost);

				auto inOpenSet = openSet.find(neighbor);
				if (inOpenSet == openSet.end())
				{
					openSet.emplace(neighbor);
				}
				else
				{
					if (inOpenSet->distance > distanceSoFar)
					{
						openSet.erase(inOpenSet);
						openSet.emplace(neighbor);
					}
				}
			}
		}

		++passes;
	}

	if (!openSet.empty())
	{
		return constructPath(openSet.begin()->value);
	}

	return std::deque<sf::Vector2i>();
}