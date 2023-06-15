#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class OpenWorldMapSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;
	};
}


