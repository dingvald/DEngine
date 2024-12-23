#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class DeathSystem : public System
	{
	public:
		using System::System;

		void update() override;
	};
}

