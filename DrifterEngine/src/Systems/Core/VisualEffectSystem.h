#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class VisualEffectSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;

	private:
		std::vector<entt::entity> _toDestroy;
	};
}


