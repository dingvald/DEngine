#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class PlayerInput : public System
	{
	public:
		using System::System;

		virtual void init() override;
		virtual void update(const float dt) override;
	};
}


