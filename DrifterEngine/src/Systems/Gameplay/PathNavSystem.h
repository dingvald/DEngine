#pragma once
#include <Systems/System.h>

namespace drft::system
{
	class PathNavSystem : public System
	{
	public:
		using System::System;

		void onUpdate(float dt) override;
	};
}