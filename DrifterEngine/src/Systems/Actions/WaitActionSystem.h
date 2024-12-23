#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class WaitActionSystem : public System
	{
	public:
		using System::System;

		void update() override;
	};
}

