#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class PickUpSystem : public System
	{
	public:
		using System::System;

	private:
		void update(const float dt) override;
	};
}


