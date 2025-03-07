#pragma once
#include "Systems/System.h"

struct CameraComponent;

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

		void smoothCameraToTarget(const TilePosition& targetPosition, CameraHandle& cam) const;
		void snapCameraToTarget(const TilePosition& targetPosition, CameraHandle& cam) const;

		entt::entity tryFindTarget() const;

	private:
		entt::entity _camera = entt::null;
	};

}


