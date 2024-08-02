#include "pch.h"
#include "VirtualChunk.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"
#include "Utility/CopyEntity.h"
#include "Utility/SaveRegistry.h"
#include "Utility/LoadRegistry.h"
#include "Conversions.h"
#include "WorldGrid.h"

#pragma optimize("", off)

using namespace drft::spatial;
using namespace std::chrono_literals;

static constexpr auto WAIT_TIME = 0.0ms; // How long to wait for async operations

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

	auto& worldGenerator = reg.ctx().get<gen::WorldGenerator&>();
	if (worldGenerator.generateChunk(_coordinate, reg) != gen::GenerationStatus::Done)
	{
		setState(ChunkState::Building);
		return ioStatus::Busy;
	}

	setState(ChunkState::Built);
	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::save(entt::registry& reg, const std::filesystem::path& filename) const
{
	entt::registry temp;

	const auto& grid = reg.ctx().get<spatial::WorldGrid&>();
	const auto entities = grid.getAllEntities(this->_coordinate);

	util::copyEntities(entities, temp, reg);
	for (auto entity : entities)
	{
		reg.destroy(entity);
	}
	reg.compact();

	if (!temp.empty())
	{
		util::saveRegistryToFile(temp, filename);
	}
	
	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::load(entt::registry& reg, const std::filesystem::path& filename) const
{
	entt::registry temp;
	util::loadRegistryFromFile(temp, filename);
	util::copyEntities(reg, temp);

	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncLoad(entt::registry& reg, const std::filesystem::path& filename)
{
	if (getState() == ChunkState::ToLoad)
	{
		setFuture(std::async(std::launch::async, &VirtualChunk::loadChunkFromFile, this, filename));
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
	_asyncRegistry = {};
	
	setState(ChunkState::Loaded);

	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncSave(entt::registry& reg, const std::filesystem::path& filename)
{
	if (getState() == ChunkState::ToSave)
	{
		const auto& grid = reg.ctx().get<spatial::WorldGrid&>();
		const auto entities = grid.getAllEntities(_coordinate);

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
		
		setFuture(std::async(std::launch::async, &VirtualChunk::saveChunkToFile, this, filename));
		std::cout << "Saving " << toString() << std::endl;
		setState(ChunkState::Saving);
	}

	auto status = getFuture().wait_for(WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}
	
	_asyncRegistry = {};

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

bool drft::spatial::VirtualChunk::saveChunkToFile(const std::filesystem::path& filename) const
{
	util::saveRegistryToFile(_asyncRegistry, filename);
	return true;
}

bool drft::spatial::VirtualChunk::loadChunkFromFile(const std::filesystem::path& filename)
{
	util::loadRegistryFromFile(_asyncRegistry, filename);
	return true;
}

std::string drft::spatial::VirtualChunk::toString() const
{
	return std::string(std::to_string(_coordinate.x) + "_" + std::to_string(_coordinate.y));
}
