#pragma once
#include "Systems/System.h"
#include <Random/Random.h>

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

	private:
		rng::Random _random;
	};
}