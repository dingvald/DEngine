#pragma once
#include "Systems/System.h"

struct SolidMaterialComponent;

namespace drft::system
{
	class IncomingForceSystem : public System
	{
	public:
		using System::System;

	private:
		void init() override;
		void update(const float dt) override;
	};
}