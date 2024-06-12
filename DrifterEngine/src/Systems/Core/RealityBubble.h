#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class RealityBubble : public System
	{
		void init() override;
		void update(const float) override;
		void onUpdateEnd() override;
	};
}



