#pragma once
#include "Systems/System.h"

struct AnimationComponent;
struct SpriteOptions;

namespace drft::system
{
	class AnimationSystem : public System
	{
	public:
		using System::System;

	private:
		void onFixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		std::vector<entt::entity> _toRemoveAnimation;
	};
}


