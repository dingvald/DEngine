#pragma once
#include <Systems/System.h>
#include <Utility/SpriteOptions.h>
#include <Spatial/TilePosition.h>

namespace drft::events
{
	struct ChangeMouseVisibilityEvent;
}

struct MouseVisibilityOptions
{
	bool shouldShowPath = true;
	bool shouldShowCursor = true;
};

namespace drft::system
{
	class MouseVisualizationSystem : public System
	{
	public:
		using System::System;

		static std::vector<TilePosition> getVisualizedPath(entt::registry& registry);
		static void changeMouseVisibility(entt::registry& registry, MouseVisibilityOptions&& options);

		const std::vector<TilePosition>& getCachedPath() const;

	private:
		void init() override;
		void start() override;
		void update() override;
		void shutdown() override;

		void showMouse();
		void hideMouse();

		void onChangeMouseVisibilityEvent(const events::ChangeMouseVisibilityEvent& ev);

		void updateMousePathVisualization();
		void refreshVisualizedPath(const std::vector<TilePosition>& path, size_t progress);

		void creatNewVisualizedPathToPlayer(TilePosition mousePosition, TilePosition playerPosition);

	private:
		entt::entity _cursor = entt::null;
		TilePosition _lastMousePosition;
		bool _shouldShowMouse = true;
		bool _hideMouse = false;

		std::vector<TilePosition> _cachedPath;
		std::vector<entt::entity> _visualizedPath;
	};
}


