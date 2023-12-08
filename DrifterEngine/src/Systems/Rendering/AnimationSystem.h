#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class AnimationSystem : public System
	{
	public:
		void init() override;
		void onStart(bool isNewgame) override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		std::vector<entt::entity> _toRemoveAnimation;
	};
}


