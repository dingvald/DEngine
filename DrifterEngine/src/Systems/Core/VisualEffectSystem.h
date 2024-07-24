#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class VisualEffectSystem : public System
	{
	public:
		using System::System;

		void onFixedUpdate() override;
	};
}


