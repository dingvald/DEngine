#include "pch.h"
#include <Events/ChunkSourceTransferCompleteEvent.h>
#include <Events/ChunkSourceTransferFailedEvent.h>
#include <Events/ChunkSourceTransferStartedEvent.h>
#include <Events/ChunkSourceTransferRequestEvent.h>
#include "ChunkManager.h"

#include "Components/PositionComponent.h"
#include <Components/ChunkSourceTrackerComponent.h>
#include <Components/CameraTargetComponent.h>

#include <EnTT/entt.h>
#include <exception>
#include <memory>
#include <Spatial/ChunkSource.h>
#include <Utility/StandardLogger.h>
#include "Systems/Helpers/GetCurrentCamera.h"
#include <SolarSystem/SolarSystem.h>

using namespace entt::literals;

const entt::id_type NULL_SOURCE_ID = "NULL_ID"_hs;

void drft::system::ChunkManager::init()
{
	_registry.on_construct<ChunkSourceTrackerComponent>().connect<&ChunkManager::onConstructChunkSourceTracker>(this);
	_registry.on_update<CameraTargetComponent>().connect<&ChunkManager::onUpdateCameraTarget>(this);

	_dispatcher.sink<events::ChunkSourceTransferRequestEvent>().connect<&ChunkManager::onChunkSourceTransferRequestEvent>(this);
}

void drft::system::ChunkManager::start()
{
	if (_chunkSourceTracker == entt::null)
	{
		_chunkSourceTracker = _registry.create();
		_registry.emplace<ChunkSourceTrackerComponent>(_chunkSourceTracker);
	}
}

void drft::system::ChunkManager::update()
{
	switch (_state)
	{
	case State::FirstUpdate:
		onFirstUpdate();
		break;
	case State::NoSource:
		onNoSource();
		break;
	case State::Transferring:
		onTransfer();
		break;
	case State::SourceReady:
		onUpdateSource();
		break;
	default:
		break;
	}
}

void drft::system::ChunkManager::shutdown()
{
	if (!_activeSource) return;

	_activeSource->shutdown(_registry, false);
}

void drft::system::ChunkManager::onFirstUpdate()
{
	LOG_MSG("<<< First Chunk Manager Update >>>")
	if (!_activeSource && !_pendingTransfer)
	{
		if (_chunkSourceTracker != entt::null)
		{
			auto& tracker = _registry.get<ChunkSourceTrackerComponent>(_chunkSourceTracker);
			_pendingTransfer.emplace(NULL_SOURCE_ID, tracker.sourceId, tracker.position);
			setState(State::Transferring);
		}
		else
		{
			setState(State::NoSource);
		}
	}
}

void drft::system::ChunkManager::onNoSource()
{
}

void drft::system::ChunkManager::onTransfer()
{
	if (!_pendingTransfer) throw std::exception("Called onTransfer without a pending transfer");

	if (!_activeSource)
	{
		_activeSource = tryCreateNewChunkSource(_pendingTransfer->newSourceId);
		if (!_activeSource)
		{
			notifyTransferFailed(_pendingTransfer->newSourceId);
			setState(State::NoSource);
			return;
		}
	}

	if (_activeSource->id() == _pendingTransfer->oldSourceId)
	{
		// Check if the new souce even exists before transferring
		if (!doesChunkSourceExist(_pendingTransfer->newSourceId))
		{
			notifyTransferFailed(_pendingTransfer->newSourceId);
			setState(State::SourceReady); // just stay using current source
			_pendingTransfer.reset();
			return;
		}

		_activeSource->shutdown(_registry, false);
		_activeSource = tryCreateNewChunkSource(_pendingTransfer->newSourceId);

		if (!_activeSource) throw std::exception("Something went terribly wrong during source transfer");
	}

	if (_activeSource->id() == _pendingTransfer->newSourceId)
	{
		_activeSource->update(_pendingTransfer->position, _registry);
		if (_activeSource->isLoadedAroundPosition(_pendingTransfer->position))
		{
			LOG_MSG("Transfer complete");
			setState(State::SourceReady);
			_registry.emplace_or_replace<ChunkSourceTrackerComponent>(_chunkSourceTracker, _pendingTransfer->newSourceId, _pendingTransfer->position);
			_pendingTransfer.reset();
			return;
		}
		else
		{
			LOG_MSG("Transfer ongoing...");
			return;
		}
	}

	setState(State::NoSource);
	_pendingTransfer.reset();
}

void drft::system::ChunkManager::onUpdateSource()
{
	if (!_activeSource)
	{
		setState(State::NoSource);
		return;
	}

	const CameraHandle camera = getCurrentCamera(_registry);
	if (!camera.isInitialized) return;

	_activeSource->update(camera.position.tile, _registry);
}

void drft::system::ChunkManager::setState(State newState)
{
	_state = newState;
	onStateChange(_state);
}

void drft::system::ChunkManager::onStateChange(State newState)
{
	switch (newState)
	{
	case State::FirstUpdate:
		// should not change into this state
		break;
	case State::NoSource:
		LOG_WARNING("No chunk source found");
		break;
	case State::Transferring:
		LOG_MSG("Requesting transfer to source id {}", _pendingTransfer->newSourceId);
		_dispatcher.trigger(events::ChunkSourceTransferStartedEvent{ _pendingTransfer->oldSourceId, _pendingTransfer->newSourceId });
		break;
	case State::SourceReady:
		_dispatcher.trigger(events::ChunkSourceTransferCompleteEvent{ _pendingTransfer->newSourceId });
		break;
	default:
		break;
	}
}

void drft::system::ChunkManager::notifyTransferFailed(entt::id_type sourceId)
{
	LOG_ERROR("Could not find chunk source with id {}", _pendingTransfer->newSourceId);
	LOG_ERROR("Transfer failed");
	_dispatcher.trigger(events::ChunkSourceTransferFailedEvent{ _pendingTransfer->newSourceId });
}

void drft::system::ChunkManager::onChunkSourceTransferRequestEvent(events::ChunkSourceTransferRequestEvent& ev)
{
	if (_pendingTransfer.has_value()) return; // TODO: should new transfer requests be ignored?

	entt::id_type oldSourceId = _activeSource ? _activeSource->id() : NULL_SOURCE_ID;
	_pendingTransfer.emplace(oldSourceId, ev.sourceId.has_value() ? ev.sourceId.value() : oldSourceId, ev.position);

	setState(State::Transferring);
}

void drft::system::ChunkManager::onConstructChunkSourceTracker(entt::registry& registry, entt::entity entity)
{
	_chunkSourceTracker = entity;
}

void drft::system::ChunkManager::onUpdateCameraTarget(entt::registry& registry, entt::entity entity)
{
	if (_chunkSourceTracker == entt::null) return;

	if (auto tracker = _registry.try_get<ChunkSourceTrackerComponent>(_chunkSourceTracker))
	{
		auto& cameraTarget = _registry.get<CameraTargetComponent>(entity);
		tracker->position = cameraTarget.position;
	}
}

drft::system::ChunkManager::SourcePtr drft::system::ChunkManager::tryCreateNewChunkSource(entt::id_type sourceId)
{
	auto& solarSytem = _registry.ctx().get<SolarSystem>("solar_system"_hs);
	if (auto generator = solarSytem.tryGetGenerator(sourceId))
	{
		return std::make_unique<spatial::ChunkSource>(_pendingTransfer->newSourceId, _serializer, *generator);
	}
	return nullptr;
}

bool drft::system::ChunkManager::doesChunkSourceExist(entt::id_type sourceId) const
{
	auto& solarSytem = _registry.ctx().get<SolarSystem>("solar_system"_hs);
	if (auto generator = solarSytem.tryGetGenerator(sourceId))
	{
		return true;
	}
	return false;
}
