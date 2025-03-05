#pragma once
#include <Systems/System.h>
#include <Utility/SpriteOptions.h>
#include <Spatial/TilePosition.h>

namespace drft::system
{
	class MouseVisualizationSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void start() override;
		void update() override;
		void shutdown() override;

		void showMouse();
		void hideMouse();

		void updateMousePathVisualization();
		void refreshVisualizedPath(const std::deque<TilePosition>& path, size_t progress);

		void creatNewVisualizedPathToPlayer(TilePosition mousePosition, TilePosition playerPosition);

	private:
		entt::entity _cursor = entt::null;
		TilePosition _lastMousePosition;
		bool _shouldShowMouse = true;
		std::vector<entt::entity> _visualizedPath;
	};
}


