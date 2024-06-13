#pragma once
#include "Systems/System.h"

struct AnimationComponent;
struct SpriteOptions;

namespace drft::system
{
	class AnimationSystem : public System
	{
	private:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		std::vector<entt::entity> _toRemoveAnimation;
	};
}


