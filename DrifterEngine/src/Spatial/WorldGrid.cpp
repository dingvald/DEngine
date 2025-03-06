#include "pch.h"
#include "WorldChunk.h"
#include "WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Utility/stdHashing.h"

using namespace drft::spatial;

static const drft::spatial::EntityList EmptyEntityList = {};

static const size_t PathingSetReserveSize = 100; // Arbitrary

namespace
{
	template<typename T, typename priority_t>
	struct PriorityQueue {
		struct Element {
			priority_t first;
			T second;

			bool operator >(const Element& other) const {
				return first > other.first;
			}
			bool operator <(const Element& other) const {
				return first < other.first;
			}
		};

		std::priority_queue<Element, std::vector<Element>,
			std::greater<Element>> elements;

		inline bool empty() const {
			return elements.empty();
		}

		inline void put(T item, priority_t priority) {
			elements.emplace(priority, item);
		}

		T get() {
			T best_item = elements.top().second;
			elements.pop();
			return best_item;
		}
	};
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
	const sf::Vector2i goal = toXY(pt2);

	std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;
	std::unordered_map<sf::Vector2i, double> costSoFar;

	PriorityQueue<sf::Vector2i, double> frontier;
	frontier.put(start, 0);

	cameFrom[start] = start;
	costSoFar[start] = 0;

	while (!frontier.empty())
	{
		const sf::Vector2i current = frontier.get();

		if (current == goal) break;

		for (auto&& neighbour : spatial::getAdjacentPoints(current))
		{
			double newCost = costSoFar[current] + costFunc(entitiesAt({ neighbour.x, neighbour.y, pt1.z }));
			if (!costSoFar.contains(neighbour) || newCost < costSoFar[neighbour])
			{
				costSoFar[neighbour] = newCost;
				double priority = newCost + distance2d(neighbour, goal);
				frontier.put(neighbour, priority);
				cameFrom[neighbour] = current;
			}
		}
	}

	std::deque<TilePosition> result;
	sf::Vector2i current = goal;
	if (!cameFrom.contains(goal))
	{
		return result; // no path
	}
	while (current != start)
	{
		result.emplace_front(current.x, current.y, pt1.z);
		current = cameFrom[current];
	}
	return result;
}