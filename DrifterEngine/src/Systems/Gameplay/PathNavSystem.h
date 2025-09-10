#pragma once
#include <Systems/System.h>

namespace drft::system
{
	class PathNavSystem : public System
	{
		using System::System;
		void update(const float dt) override;
	};
}