#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class VisualEffectSystem : public System
	{
		using System::System;

		void fixedUpdate() override;
		void shutdown() override;
	};
}


