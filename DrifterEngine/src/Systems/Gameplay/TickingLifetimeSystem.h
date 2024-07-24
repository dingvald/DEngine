#pragma once
#include "Systems/System.h"
#include "Events/GameTickEvent.h"

namespace drft::system
{
	class TickingLifetimeSystem : public System
	{
	public:
		using System::System;

		void init() override;

	private:
		void onGameTickEvent(events::GameTickEvent& ev);
	};
}


