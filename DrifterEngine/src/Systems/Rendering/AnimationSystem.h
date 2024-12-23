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
		void update() override;
		void updateEnd() override;

	private:
		std::vector<entt::entity> _toRemoveAnimation;
	};
}


