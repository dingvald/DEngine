#pragma once
#include "Systems/System.h"
#include "Events/ItemBreakEvent.h"

namespace drft::system
{
	class ItemDurabilitySystem : public System
	{
	public:
		void init() override;

	private:
		void onItemBreakEvent(events::ItemBreakEvent& ev);
	};
}

