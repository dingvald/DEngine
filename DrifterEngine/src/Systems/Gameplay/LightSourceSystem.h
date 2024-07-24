#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class LightSourceSystem : public System
	{
	public:
		using System::System;

		void onFixedUpdate() override;
		void onFixedUpdateEnd() override;
	};
}



