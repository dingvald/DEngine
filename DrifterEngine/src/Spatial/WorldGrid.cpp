#include "pch.h"
#include "WorldChunk.h"
#include "WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Utility/stdHashing.h"

using namespace drft::spatial;

static const drft::spatial::EntityList EmptyEntityList = {};

void drft::spatial::WorldGrid::placeEntity(entt::entity entity, TilePosition tilePosition)
{	
	if (entity == entt::null) return;

	auto chunkPosition = toChunkSpace(tilePosition);
	auto localPosition = toChunkLocalSpace(tilePosition);

	if (!_chunks.contains(chunkPosition))
	{
		_chunks.emplace(chunkPosition, WorldChunk{ ChunkDimensions });
	}
	_chunks.at(chunkPosition).placeEntity(entity, localPosition);
	_entityPositions[entity] = tilePosition;
}

void drft::spatial::WorldGrid::removeEntity(entt::entity entity)
{
	if (entity == entt::null) return;
	if (!_entityPositions.contains(entity)) return;

	TilePosition tilePosition = getPosition(entity);

	auto chunkPosition = toChunkSpace(tilePosition);
	auto localPosition = toChunkLocalSpace(tilePosition);

	if (!_chunks.contains(chunkPosition)) return;

	_chunks.at(chunkPosition).removeEntity(entity, localPosition);
	_entityPositions.erase(entity);
}

void drft::spatial::WorldGrid::moveEntity(entt::entity entity, TilePosition toTilePosition)
{
	this->removeEntity(entity);
	this->placeEntity(entity, toTilePosition);
}

drft::TilePosition drft::spatial::WorldGrid::getPosition(entt::entity entity) const
{
	return _entityPositions.at(entity);
}

const EntityList& drft::spatial::WorldGrid::entitiesAt(TilePosition tilePosition) const
{
	auto chunkPosition = toChunkSpace(tilePosition);
	auto localPosition = toChunkLocalSpace(tilePosition);

	if (!_chunks.contains(chunkPosition)) {
		return EmptyEntityList;
	}

	return _chunks.at(chunkPosition).entitiesAt(localPosition);;
}

EntityList drft::spatial::WorldGrid::entitiesAt(TilePosition tilePosition, std::function<bool(entt::entity)> filterFunc) const
{
	auto chunkPosition = toChunkSpace(tilePosition);
	auto localPosition = toChunkLocalSpace(tilePosition);

	if (!_chunks.contains(chunkPosition)) {
		return EntityList{};
	}

	std::vector<entt::entity> result;
	const auto& entities = _chunks.at(chunkPosition).entitiesAt(localPosition);
	result.reserve(result.size());

	for (auto entity : entities)
	{
		if (filterFunc(entity))
		{
			result.emplace_back(entity);
		}
	}

	return result;
}

void drft::spatial::WorldGrid::removeChunk(ChunkPosition coordinate)
{
	if (_chunks.contains(coordinate))
	{
		_chunks.erase( coordinate );
	}
}

EntityList drft::spatial::WorldGrid::getAllEntities(ChunkPosition coordinate) const
{
	if (!_chunks.contains(coordinate))
	{
		return EntityList{};
	}
	return _chunks.at(coordinate).getAllEntities();
}

std::vector<entt::entity> drft::spatial::WorldGrid::castRay(TilePosition origin, TilePosition destination) const
{
	std::vector<entt::entity> result;

	const auto points = spatial::getLine3d(origin, destination);
	result.reserve(points.size());
	for (const sf::Vector3i& point : points)
	{
		auto& entities = entitiesAt(TilePosition{ point });
		result.insert(result.end(), entities.begin(), entities.end());
	}
	return result;
}

std::vector<entt::entity> drft::spatial::WorldGrid::castRay(TilePosition origin, TilePosition destination, std::function<bool(entt::entity)> filterFunc) const
{
	std::vector<entt::entity> result;

	const auto points = spatial::getLine3d(origin, destination);
	result.reserve(points.size());
	for (const sf::Vector3i& point : points)
	{
		auto& entities = entitiesAt(TilePosition{point});
		for (auto&& entity : entities)
		{
			if (!filterFunc(entity)) continue;
			result.push_back(entity);
		}
	}
	return result;
}

std::deque<sf::Vector3i> drft::spatial::WorldGrid::getPath(TilePosition pt1, TilePosition pt2, Heuristic costFunc) const
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

	int z = pt1.z;
	sf::Vector2i pt1_2d = { pt1.x, pt1.y };
	sf::Vector2i pt2_2d = { pt2.x, pt2.y };

	std::unordered_set<sf::Vector2i> closedSet;
	std::set<Node> openSet;
	std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;
	auto constructPath = [z, &cameFrom](sf::Vector2i endPosition) -> std::deque<sf::Vector3i>
	{
		sf::Vector2i currentPosition = endPosition;
		std::deque<sf::Vector3i> path;
		while (currentPosition != cameFrom[currentPosition])
		{
			path.push_front({currentPosition.x, currentPosition.y, z});
			currentPosition = cameFrom[currentPosition];
		}

		return path;
	};
	
	openSet.emplace(Node(pt1_2d,0,0));
	cameFrom[pt1_2d] = pt1_2d;

	const int LIMIT = 50;
	int passes = 0;
	while (!openSet.empty() && passes < LIMIT)
	{
		Node currentNode = *(openSet.begin());
		if (currentNode.value == pt2_2d) return constructPath(currentNode.value);
		openSet.erase(openSet.begin());
		closedSet.emplace(currentNode.value);

		// check neighbors
		for (int y = currentNode.value.y - 1; y <= currentNode.value.y + 1; ++y)
		{
			for (int x = currentNode.value.x - 1; x <= currentNode.value.x + 1; ++x)
			{
				if (closedSet.contains({x,y})) continue;
				cameFrom[{x, y}] = currentNode.value;
				if (sf::Vector2i(x,y) == pt2_2d) return constructPath(sf::Vector2i(x,y));

				int distanceSoFar = currentNode.distance + 1;
				int distanceFromTarget = static_cast<int>(std::sqrtf(std::powf(pt2.x - x, 2.f) + std::powf(pt2.y - y, 2.f)));
				const auto entities = entitiesAt({ x,y,z });
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

	return std::deque<sf::Vector3i>();
}