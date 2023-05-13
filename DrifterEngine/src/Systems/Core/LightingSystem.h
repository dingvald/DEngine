#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class LightingSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
	};
}


