#include "pch.h"
#include "VirtualChunk.h"
#include "Utility/CopyEntity.h"
#include "Utility/TestEntities.h"
#include "Utility/SaveRegistry.h"
#include"Utility/LoadRegistry.h"
#include "Conversions.h"
#include "WorldGrid.h"

using namespace drft::spatial;
using namespace std::chrono_literals;

static constexpr auto WAIT_TIME = 3ms; // How long to wait for async operations

void drft::spatial::VirtualChunk::setState(ChunkState state)
{
	this->_state = state;
}

ChunkState VirtualChunk::getState() const
{
	return this->_state;
}

ioStatus drft::spatial::VirtualChunk::build(entt::registry& reg)
{
	if (getState() == ChunkState::ToBuild)
	{
		std::cout << "Building " << toString() << std::endl;
		setState(ChunkState::Building);
	}
	auto origin = toTileSpace(_coordinate);
	auto bounds = sf::Vector2i{ CHUNK_WIDTH, CHUNK_HEIGHT };
	util::buildMany("Tree", 300, { origin.x, origin.y, bounds.x, bounds.y }, reg);
	util::buildMany("Zombie", 10, { origin.x, origin.y, bounds.x, bounds.y }, reg);
	
	setState(ChunkState::Built);

	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::save(entt::registry& reg, const char* filepath)
{
	entt::registry temp;
	std::string filename = this->toString();

	const auto& grid = reg.ctx().get<spatial::WorldGrid&>();
	const auto entities = grid.getAllEntities(this->_coordinate);

	util::copyEntities(entities, temp, reg);
	if (!temp.empty())
	{
		util::saveRegistryToFile(temp, filepath, filename);
	}
	
	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::load(entt::registry& reg, const char* filepath)
{
	entt::registry temp;
	std::string filename = this->toString();
	util::loadRegistryFromFile(temp, filepath, filename);

	util::copyEntities(reg, temp);

	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncLoad(entt::registry& reg, const char* filepath)
{
	if (getState() == ChunkState::ToLoad)
	{
		setFuture(std::async(std::launch::async, &VirtualChunk::loadChunkFromFile, this, filepath));
		std::cout << "Loading " << toString() << std::endl;
		setState(ChunkState::Loading);
	}

	auto status = getFuture().wait_for(WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}

	util::copyEntities(reg, _asyncRegistry);

	reg.compact();
	_asyncRegistry.clear();
	_asyncRegistry.compact();
	
	setState(ChunkState::Loaded);

	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncSave(entt::registry& reg, const char* filepath)
{
	if (getState() == ChunkState::ToSave)
	{
		const auto& grid = reg.ctx().get<spatial::WorldGrid&>();
		const auto entities = grid.getAllEntities(this->_coordinate);

		if (entities.empty())
		{
			std::cout << "No need to save " << toString() << " chunk empty" << std::endl;
			setState(ChunkState::Saved);
			return ioStatus::Done;
		}
		
		util::copyEntities(entities, _asyncRegistry, reg);
		
		for (auto e : entities)
		{
			reg.destroy(e);
		}
		reg.compact();
		
		setFuture(std::async(std::launch::async, &VirtualChunk::saveChunkToFile, this, filepath));
		std::cout << "Saving " << toString() << std::endl;
		setState(ChunkState::Saving);
	}

	auto status = getFuture().wait_for(WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}
	
	_asyncRegistry.clear();

	setState(ChunkState::Saved);

	return ioStatus::Done;
}

void VirtualChunk::setFuture(std::shared_future<bool> future)
{
	this->_future = future;
}

const std::shared_future<bool>& VirtualChunk::getFuture() const
{
	return this->_future;
}

bool drft::spatial::VirtualChunk::saveChunkToFile(const char* filepath) const
{
	std::string filename = this->toString();
	util::saveRegistryToFile(_asyncRegistry, filepath, filename);

	return true;
}

bool drft::spatial::VirtualChunk::loadChunkFromFile(const char* filepath)
{
	std::string filename = this->toString();
	util::loadRegistryFromFile(_asyncRegistry, filepath, filename);

	return true;
}

std::string drft::spatial::VirtualChunk::toString() const
{
	return std::string(std::to_string(_coordinate.x) + "_" + std::to_string(_coordinate.y));
}
