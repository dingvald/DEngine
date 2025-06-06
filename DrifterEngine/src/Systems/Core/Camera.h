#pragma once
#include "Systems/System.h"

struct CameraComponent;

namespace drft::events
{
	struct ChunkSourceTransferCompleteEvent;
}

namespace drft
{
	struct TilePosition;
}

namespace drft::system
{
	struct CameraHandle;

	class Camera : public System
	{
	public:
		using System::System;

	private:
		void start() override;
		void updateEnd() override;
		void render(sf::RenderTarget& target) override;
		void shutdown() override;

		void smoothCameraToTarget(sf::Vector2f targetPosition, CameraHandle& cam) const;
		void snapCameraToTarget(TilePosition targetPosition, CameraHandle& cam) const;
		void onChunkSourceTransferCompleteEvent(events::ChunkSourceTransferCompleteEvent& ev);

		entt::entity tryFindTarget() const;

	private:
		entt::entity _camera = entt::null;
		bool _justCompletedTransfer = false;
	};

}


