#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class EffectSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;

	private:
		std::vector<entt::entity> _toDestroy;
	};
}


