#include "pch.h"
#include "ChunkManager.h"
#include <Engine/CommonEngineDirectories.h>

#include <Events/ChunkSourceTransferCompleteEvent.h>
#include <Events/ChunkSourceTransferRequestEvent.h>
#include <Events/ChunkSourceTransferInProgressEvent.h>

#include "Spatial/WorldGrid.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "Components/CameraComponent.h"
#include "Components/PositionComponent.h"

#include "Systems/Helpers/GetCurrentCamera.h"
#include <Utility/StandardLogger.h>

const entt::id_type NULL_SOURCE_ID = entt::hashed_string{"NULL_ID"};

void drft::system::ChunkManager::init()
{
	_dispatcher.sink<events::ChunkSourceTransferRequestEvent>().connect<&ChunkManager::onRequestChunkSourceChangeEvent>(this);
}

void drft::system::ChunkManager::update()
{
	switch (_state)
	{
	case drft::system::ChunkManager::State::NoSource:
		onNoSource();
		break;
	case drft::system::ChunkManager::State::Transferring:
		onTransfer();
		break;
	case drft::system::ChunkManager::State::SourceReady:
		onUpdateSource();
		break;
	default:
		break;
	}
}

void drft::system::ChunkManager::shutdown()
{
	if (!_activeSource) return;

	_activeSource->shutdown(_registry);
}

void drft::system::ChunkManager::onNoSource()
{
	
}

void drft::system::ChunkManager::onTransfer()
{
	if (!_activeSource)
	{
		_activeSource = std::make_unique<spatial::ChunkSource>(_pendingTransfer->newSourceId, _serializer);
	}

	if (_activeSource->id() == _pendingTransfer->oldSourceId)
	{
		_activeSource->shutdown(_registry);
		_activeSource.reset();
		_activeSource = std::make_unique<spatial::ChunkSource>(_pendingTransfer->newSourceId, _serializer);
	}

	if (_activeSource->id() == _pendingTransfer->newSourceId)
	{
		if (_activeSource->isReady())
		{
			setState(State::SourceReady);
			_pendingTransfer.reset();
		}
	}
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
	case drft::system::ChunkManager::State::NoSource:
		LOG_WARNING("No chunk source found");
		break;
	case drft::system::ChunkManager::State::Transferring:
		_dispatcher.trigger(events::ChunkSourceTransferInProgressEvent{ _pendingTransfer->oldSourceId, _pendingTransfer->newSourceId });
		break;
	case drft::system::ChunkManager::State::SourceReady:
		_dispatcher.trigger(events::ChunkSourceTransferCompleteEvent{ _pendingTransfer->newSourceId });
		break;
	default:
		break;
	}
}

void drft::system::ChunkManager::onRequestChunkSourceChangeEvent(events::ChunkSourceTransferRequestEvent& ev)
{
	if (_pendingTransfer.has_value()) return; // TODO: should new transfer requests be ignored?

	entt::id_type oldSourceId = _activeSource ? _activeSource->id() : NULL_SOURCE_ID;
	_pendingTransfer.emplace(oldSourceId, ev.sourceId);

	setState(State::Transferring);
}
