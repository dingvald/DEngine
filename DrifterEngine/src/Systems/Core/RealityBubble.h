#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class RealityBubble : public System
	{
	public:
		virtual void init() override;
		virtual void update(const float) override;
		virtual void onUpdateEnd() override;
	};
}



