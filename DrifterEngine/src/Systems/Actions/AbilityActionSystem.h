#pragma once

#include "Systems/System.h"

namespace drft
{
	class IAbility;
}

namespace drft::system
{
	class AbilityActionSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update() override;
	};
}