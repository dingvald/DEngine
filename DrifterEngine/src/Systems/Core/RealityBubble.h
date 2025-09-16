#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class RealityBubble : public System
	{
	public:
		using System::System;

		void updateBegin(const float dt) override;
	};
}



