#include "pch.h"
#include "PlayerTransferSystem.h"

#include <Events/PlayerTransferRequestEvent.h>
#include <Events/ChunkSourceTransferCompleteEvent.h>
#include <Events/ChunkSourceTransferStartedEvent.h>
#include <Events/ChunkSourceTransferRequestEvent.h>
#include <Events/ChunkSourceTransferFailedEvent.h>

#include <Systems/Helpers/GetPlayerHandle.h>

void drft::system::PlayerTransferSystem::init()
{
	_dispatcher.sink<events::PlayerTransferRequestEvent>().connect<&PlayerTransferSystem::onPlayerTransferRequestEvent>(this);
	_dispatcher.sink<events::ChunkSourceTransferCompleteEvent>().connect<&PlayerTransferSystem::onChunkSourceTransferCompleteEvent>(this);
	_dispatcher.sink<events::ChunkSourceTransferStartedEvent>().connect<&PlayerTransferSystem::onChunkSourceTransferStartedEvent>(this);
	_dispatcher.sink<events::ChunkSourceTransferFailedEvent>().connect<&PlayerTransferSystem::onChunkSourceTransferFailedEvent>(this);
}

void drft::system::PlayerTransferSystem::update(const float dt)
{
}

void drft::system::PlayerTransferSystem::onPlayerTransferRequestEvent(events::PlayerTransferRequestEvent& ev)
{
	auto player = getPlayerHandle(_registry);
	_pendingTransfer = PendingPlayerTransfer{ ev.sourceId, ev.position, player.get<PositionComponent>(), player};
	_dispatcher.trigger(events::ChunkSourceTransferRequestEvent{ ev.sourceId, ev.position });
}

void drft::system::PlayerTransferSystem::onChunkSourceTransferStartedEvent(events::ChunkSourceTransferStartedEvent& ev)
{
	if (!_pendingTransfer) return; // Transfer didn't originate from player

	// Remove position component or player will be sucked away by chunk saving
	_pendingTransfer->player.remove<PositionComponent>();
}

void drft::system::PlayerTransferSystem::onChunkSourceTransferCompleteEvent(events::ChunkSourceTransferCompleteEvent& ev)
{
	if (!_pendingTransfer) return; // Transfer didn't originate from player

	_pendingTransfer->positionComponent.tile = _pendingTransfer->targetPosition;
	_pendingTransfer->player.emplace_or_replace<PositionComponent>(_pendingTransfer->positionComponent);
}

void drft::system::PlayerTransferSystem::onChunkSourceTransferFailedEvent(events::ChunkSourceTransferFailedEvent& ev)
{
	if (!_pendingTransfer) return; // Transfer didn't originate from player

	_pendingTransfer->player.emplace_or_replace<PositionComponent>(_pendingTransfer->positionComponent);
}
