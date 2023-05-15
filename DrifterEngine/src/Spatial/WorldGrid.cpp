#include "pch.h"
#include "WorldChunk.h"
#include "WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Utility/stdHashing.h"

using namespace drft::spatial;

void drft::spatial::WorldGrid::placeEntity(const entt::entity entity, const sf::Vector2i worldPosition, int layer)
{
	auto chunkCoordinate = toChunkCoordinate(worldPosition);
	auto localPosition = toLocalChunkSpace(worldPosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair))
	{
		_chunks[keyablePair] = std::make_unique<WorldChunk>(CHUNK_WIDTH, CHUNK_HEIGHT);
	}
	_chunks[keyablePair]->placeEntity(entity, localPosition, layer);
	_entityPositions[entity] = { worldPosition.x, worldPosition.y, layer };
}

entt::entity drft::spatial::WorldGrid::removeEntity(const entt::entity entity)
{
	if (entity == entt::null) return entity;

	entt::entity result = entt::null;

	sf::Vector2i worldPosition = getPosition(entity);
	int layer = static_cast<int>(getLayer(entity));

	auto chunkCoordinate = toChunkCoordinate(worldPosition);
	auto localPosition = toLocalChunkSpace(worldPosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair))
	{
		assert(false);
		return result;
	}
	result = _chunks.at(keyablePair)->removeEntity(entity, localPosition, layer);
	_entityPositions.erase(entity);

	return result;
}

bool drft::spatial::WorldGrid::moveEntity(const entt::entity entity, const sf::Vector2i toWorldPosition, int layer)
{
	this->removeEntity(entity);
	this->placeEntity(entity, toWorldPosition, layer);

	return true;
}

const sf::Vector2i drft::spatial::WorldGrid::getPosition(const entt::entity entity) const
{
	auto vec3 = _entityPositions.at(entity);
	return { vec3.x, vec3.y };
}

const int drft::spatial::WorldGrid::getLayer(const entt::entity entity) const
{
	auto vec3 = _entityPositions.at(entity);
	return vec3.z;
}

const EntityList drft::spatial::WorldGrid::entitiesAt(const sf::Vector2i worldPosition, const int layer) const
{
	auto chunkCoordinate = toChunkCoordinate(worldPosition);
	auto localPosition = toLocalChunkSpace(worldPosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair)) {
		return EntityList{};
	}

	return _chunks.at(keyablePair)->entitiesAt(localPosition, layer);
}

const EntityList drft::spatial::WorldGrid::entitiesAt(const sf::Vector2i tilePosition) const
{
	auto chunkCoordinate = toChunkCoordinate(tilePosition);
	auto localPosition = toLocalChunkSpace(tilePosition);
	auto keyablePair = std::make_pair(chunkCoordinate.x, chunkCoordinate.y);

	if (!_chunks.contains(keyablePair)) {
		return EntityList{};
	}
	EntityList entities;
	for (int layer = 0; layer < Layer::Total; ++layer)
	{
		auto e = _chunks.at(keyablePair)->entitiesAt(localPosition, layer);
		entities.insert(entities.end(), e.begin(), e.end());
	}

	return entities;
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
	if (_chunks.contains({ coordinate.x, coordinate.y }))
	{
		return _chunks.at({ coordinate.x, coordinate.y })->getAllEntities();
	}
	return std::vector<entt::entity>();
}

std::deque<sf::Vector2i> drft::spatial::WorldGrid::getPath(sf::Vector2i pt1, sf::Vector2i pt2, heuristic costFunc) const
{
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
	auto checkBlocked = [this](sf::Vector2i position) -> bool
	{
		return (!entitiesAt(position, spatial::Layer::Blocking).empty());
	};
	
	openSet.emplace(Node(pt1,0,0));
	cameFrom[pt1] = pt1;

	while (!openSet.empty())
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
				if (checkBlocked({ x,y }))
				{
					if (sf::Vector2i(x, y) == pt2) return constructPath({ x,y });
					else continue;
				}

				int distance = currentNode.distance + 1;
				int cost = distance + std::pow(pt2.x - x, 2) + std::pow(pt2.y - y, 2) + costFunc(*this, { x,y });

				Node neighbor = Node({ x,y }, distance, cost);

				auto inOpenSet = openSet.find(neighbor);
				if (inOpenSet == openSet.end())
				{
					openSet.emplace(neighbor);
				}
				else
				{
					if (inOpenSet->distance > distance)
					{
						openSet.erase(inOpenSet);
						openSet.emplace(neighbor);
					}
				}
			}
		}
	}

	return std::deque<sf::Vector2i>();
}