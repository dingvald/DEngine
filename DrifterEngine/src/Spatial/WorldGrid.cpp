#include "pch.h"
#include "WorldChunk.h"
#include "WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Utility/stdHashing.h"

using namespace drft::spatial;

static const drft::spatial::EntityList EmptyEntityList = {};

namespace
{
	
	struct Node
	{
		Node() = default;
		Node(sf::Vector2i coordinate)
			: coordinate(coordinate) {}

		sf::Vector2i coordinate;
		float h = 0;
		float g = 0;
		float getScore() const
		{
			return h + g;
		}

		bool operator ==(const Node& other)
		{
			return coordinate == other.coordinate;
		}

		bool operator !=(const Node& other)
		{
			return coordinate != other.coordinate;
		}
	};

	using NodeSet = std::vector<Node>;

	Node* findNodeInList(NodeSet& nodes, sf::Vector2i coordinate)
	{
		for (auto&& node : nodes)
		{
			if (node.coordinate == coordinate)
			{
				return &node;
			}
		}
		return nullptr;
	}
}

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

std::deque<drft::TilePosition> drft::spatial::WorldGrid::getPath(TilePosition pt1, TilePosition pt2, Heuristic costFunc) const
{
	const sf::Vector2i start = toXY(pt1);
	const sf::Vector2i end = toXY(pt2);

	NodeSet openSet;
	NodeSet closedSet;

	std::unordered_map<sf::Vector2i, Node> connections;

	Node current = { start };
	connections[start] = current;

	openSet.emplace_back(start);

	while (!openSet.empty())
	{
		auto currentItr = openSet.begin();
		current = *currentItr;

		for (auto it = openSet.begin(); it != openSet.end(); ++it)
		{
			auto& node = *it;
			if (node.getScore() <= current.getScore())
			{
				current = node;
				currentItr = it;
			}
		}

		if (current.coordinate == end) break;

		closedSet.emplace_back(current);
		openSet.erase(currentItr);

		for (int y = current.coordinate.y - 1; y <= current.coordinate.y + 1; ++y)
		{
			for (int x = current.coordinate.x - 1; x <= current.coordinate.x + 1; ++x)
			{
				const sf::Vector2i neighbourCoordinates = { x, y };
				if (findNodeInList(closedSet, neighbourCoordinates)) continue;

				float totalCost = current.g + 1.f + distance2d(neighbourCoordinates, end);

				Node* neighbour = findNodeInList(openSet, neighbourCoordinates);
				if (!neighbour)
				{
					Node newNode = { neighbourCoordinates };
					connections[neighbourCoordinates] = current;
					newNode.g = totalCost;
					newNode.h = costFunc(entitiesAt({ neighbourCoordinates.x, neighbourCoordinates.y, pt1.z }));
					openSet.emplace_back(std::move(newNode));
				}
				else if (totalCost < neighbour->g)
				{
					connections[neighbourCoordinates] = current;
					neighbour->g = totalCost;
				}
			}
		}
	}

	std::deque<TilePosition> result;
	while (current != connections.at(current.coordinate))
	{
		result.emplace_front(current.coordinate.x, current.coordinate.y, pt1.z);
		current = connections.at(current.coordinate);
	}

	return result;
}