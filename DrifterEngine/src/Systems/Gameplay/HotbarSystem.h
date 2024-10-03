#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class HotbarSystem : public System
	{
	public:
		using System::System;

		virtual void init() override;
		virtual void onStart() override;
		virtual void onUpdate(float dt) override;
		virtual void onUpdateEnd() override;
	};
}


