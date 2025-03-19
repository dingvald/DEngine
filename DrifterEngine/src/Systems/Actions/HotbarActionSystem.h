#pragma once
#include "Systems/System.h"

namespace drft::events
{
	struct HUDHotbarPressedEvent;
}

namespace drft::system
{
	class HotbarActionSystem : public System
	{
	public:
		using System::System;

	private:
		virtual void init() override;
		virtual void start() override;
		virtual void update() override;

		void onHUDHotbarPressedEvent(const events::HUDHotbarPressedEvent& ev);
	};
}


