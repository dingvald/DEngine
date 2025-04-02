#pragma once
#include "Systems/System.h"
#include <Spatial/TilePosition.h>
#include <Components/PositionComponent.h>

namespace drft::events
{
	struct PlayerTransferRequestEvent;
	struct ChunkSourceTransferStartedEvent;
	struct ChunkSourceTransferCompleteEvent;
	struct ChunkSourceTransferFailedEvent;
}

namespace drft::system
{
	class PlayerTransferSystem : public System
	{
	private:
		struct PendingPlayerTransfer
		{
			entt::id_type sourceId;
			TilePosition targetPosition;
			PositionComponent positionComponent;
			entt::handle player;
		};
	public:
		using System::System;

	private:
		virtual void init() override;
		virtual void update() override;

		void onPlayerTransferRequestEvent(events::PlayerTransferRequestEvent& ev);
		void onChunkSourceTransferStartedEvent(events::ChunkSourceTransferStartedEvent& ev);
		void onChunkSourceTransferCompleteEvent(events::ChunkSourceTransferCompleteEvent& ev);
		void onChunkSourceTransferFailedEvent(events::ChunkSourceTransferFailedEvent& ev);

	private:
		std::optional<PendingPlayerTransfer> _pendingTransfer;
	};
}

