#pragma once
#include "Systems/System.h"

struct AnimationComponent;

namespace drft::system
{
	class AnimationSystem : public System
	{
	private:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

		int moveToNextFrame(const AnimationComponent& animation);
		int moveToPreviousFrame(const AnimationComponent& animation);

	private:
		std::vector<entt::entity> _toRemoveAnimation;
	};
}


