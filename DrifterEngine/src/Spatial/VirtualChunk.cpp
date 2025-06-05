#include "pch.h"
#include "VirtualChunk.h"
#include "ProcGen/IChunkGenerator.h"
#include "Utility/CopyEntity.h"
#include "WorldGrid.h"
#include <Utility/ChunkSerializer.h>
#include <Utility/StandardLogger.h>

using namespace drft::spatial;
using namespace entt::literals;
using namespace std::chrono_literals;

static constexpr auto ASYNC_WAIT_TIME = 0.00ms; // How long to wait for async operations

void drft::spatial::VirtualChunk::setState(ChunkState state)
{
	this->_state = state;
}

ChunkState VirtualChunk::getState() const
{
	return this->_state;
}

drft::ChunkPosition drft::spatial::VirtualChunk::getPosition() const
{
	return _coordinate;
}

ioStatus drft::spatial::VirtualChunk::build(entt::registry& reg, IChunkGenerator& generator)
{
	if (getState() == ChunkState::ToBuild)
	{
		setState(ChunkState::Building);
	}

	GenerationState result = generator.generateChunk(_coordinate, reg);
	if (result == GenerationState::Failed)
	{
		LOG_ERROR("Could not build from source {} at position {} {} {}", _sourceId, _coordinate.x, _coordinate.y, _coordinate.z);
		setState(ChunkState::Built);
		return ioStatus::Done;
	}
	if (result == GenerationState::Generating)
	{
		setState(ChunkState::Building);
		return ioStatus::Busy;
	}

	setState(ChunkState::Built);
	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncLoad(entt::registry& reg, ChunkSerializer& serializer)
{
	if (getState() == ChunkState::ToLoad)
	{
		auto future = serializer.queueForLoad({ _sourceId, _coordinate }, _asyncRegistry);
		setFuture(std::move(future));
		setState(ChunkState::Loading);
	}

	auto status = getFuture().wait_for(ASYNC_WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}

	util::copyEntities(reg, _asyncRegistry);
	_asyncRegistry = {};
	
	setState(ChunkState::Loaded);

	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncSave(entt::registry& reg, ChunkSerializer& serializer)
{
	if (getState() == ChunkState::ToSave)
	{
		const auto& grid = reg.ctx().get<spatial::WorldGrid&>();
		const auto entities = grid.getAllEntities(_coordinate);

		if (entities.empty())
		{
			setState(ChunkState::Saved);
			return ioStatus::Done;
		}
		
		util::copyEntities(entities, _asyncRegistry, reg);
		
		reg.destroy(entities.begin(), entities.end());

		reg.compact();
		
		auto future = serializer.queueForSave({ _sourceId, _coordinate }, _asyncRegistry);
		setFuture(std::move(future));
		setState(ChunkState::Saving);
	}

	auto status = getFuture().wait_for(ASYNC_WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}
	
	_asyncRegistry = {};

	setState(ChunkState::Saved);

	return ioStatus::Done;
}

void VirtualChunk::setFuture(std::future<void> future)
{
	_future = std::move(future);
}

const std::future<void>& VirtualChunk::getFuture() const
{
	return _future;
}

std::string drft::spatial::VirtualChunk::toString() const
{
	return std::string(std::to_string(_coordinate.x) + "_" + std::to_string(_coordinate.y) + "_" + std::to_string(_coordinate.z));
}
