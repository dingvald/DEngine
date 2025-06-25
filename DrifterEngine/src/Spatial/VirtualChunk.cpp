#include "pch.h"
#include "VirtualChunk.h"
#include "Utility/CopyEntity.h"
#include "WorldGrid.h"
#include <Utility/ChunkSerializer.h>
#include <ProcGen/ChunkGenerator.h>
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

ioStatus drft::spatial::VirtualChunk::asyncBuild(entt::registry& reg, ChunkGenerator& generator)
{
	if (getState() == ChunkState::ToBuild)
	{
		auto future = generator.queueForGeneration(_coordinate, entt::registry{});
		setFuture(std::move(future));
		setState(ChunkState::Building);
	}

	auto status = getFuture().wait_for(ASYNC_WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}

	entt::registry result = getFuture().get();
	util::copyEntities(reg, result);

	setState(ChunkState::Built);
	return ioStatus::Done;
}

ioStatus drft::spatial::VirtualChunk::asyncLoad(entt::registry& reg, ChunkSerializer& serializer)
{
	if (getState() == ChunkState::ToLoad)
	{
		auto future = serializer.queueForLoad({ _sourceId, _coordinate }, entt::registry{});
		setFuture(std::move(future));
		setState(ChunkState::Loading);
	}

	auto status = getFuture().wait_for(ASYNC_WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}

	entt::registry result = getFuture().get();
	util::copyEntities(reg, result);
	
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
		
		entt::registry asyncRegistry;
		util::copyEntities(entities, asyncRegistry, reg);
		
		reg.destroy(entities.begin(), entities.end());
		reg.compact();
		
		auto future = serializer.queueForSave({ _sourceId, _coordinate }, std::move(asyncRegistry));
		setFuture(std::move(future));
		setState(ChunkState::Saving);
	}

	auto status = getFuture().wait_for(ASYNC_WAIT_TIME);
	if (status != std::future_status::ready)
	{
		return ioStatus::Busy;
	}

	setState(ChunkState::Saved);

	return ioStatus::Done;
}

void VirtualChunk::setFuture(FutureRegistry&& future)
{
	_future = std::move(future);
}

VirtualChunk::FutureRegistry& VirtualChunk::getFuture()
{
	return _future;
}

std::string drft::spatial::VirtualChunk::toString() const
{
	return std::string(std::to_string(_coordinate.x) + "_" + std::to_string(_coordinate.y) + "_" + std::to_string(_coordinate.z));
}
