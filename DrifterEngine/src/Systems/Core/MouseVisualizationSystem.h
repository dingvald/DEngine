#pragma once
#include <Systems/System.h>
#include <Utility/SpriteOptions.h>

namespace drft::system
{
	class MouseVisualizationSystem : public System
	{
	public:
		using System::System;

		void onStart() override;
		void onUpdate(float dt) override;
		void shutdown() override;

	private:
		SpriteOptions _cursorSprite;
		entt::entity _cursor = entt::null;
	};
}


