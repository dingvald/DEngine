#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class LightSourceSystem : public System
	{
	public:
		void init() override;
		void fixedUpdate() override;
		void onFixedUpdateEnd() override;
	};
}



