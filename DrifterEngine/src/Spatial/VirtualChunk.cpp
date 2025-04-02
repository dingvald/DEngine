#include "pch.h"
#include "VirtualChunk.h"
#include "ProcGen/IChunkGeneratorProvider.h"
#include "ProcGen/IChunkGenerator.h"
#include "Utility/CopyEntity.h"
#include "Utility/SaveRegistry.h"
#include "Utility/LoadRegistry.h"
#include "Conversions.h"
#include "WorldGrid.h"
#include <Utility/ChunkSerializer.h>
#include <Utility/StandardLogger.h>


using namespace drft::spatial;
using namespace entt::literals;
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

drft::ChunkPosition drft::spatial::VirtualChunk::getPosition() const
{
	return _coordinate;
}

ioStatus drft::spatial::VirtualChunk::build(entt::registry& reg)
{
	if (getState() == ChunkState::ToBuild)
	{
		setState(ChunkState::Building);
	}

	auto& generatorProvider = reg.ctx().get<IChunkGeneratorProvider&>("solar_system"_hs);
	IChunkGenerator* chunkGenerator = generatorProvider.get(_sourceId);
	if (!chunkGenerator)
	{
		LOG_ERROR("Could not get requested chunk source id");
		return ioStatus::Done;
	}
	else if (chunkGenerator->generateChunk(_coordinate, reg) != GenerationState::Complete)
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
		setFuture(serializer.queueForLoad({ _sourceId, _coordinate }, _asyncRegistry));
		setState(ChunkState::Loading);
	}

	auto status = getFuture().wait_for(WAIT_TIME);
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
		
		setFuture(serializer.queueForSave({ _sourceId, _coordinate }, _asyncRegistry));
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
