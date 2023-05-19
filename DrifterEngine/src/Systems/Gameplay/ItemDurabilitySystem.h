#pragma once
#include "Systems/System.h"
#include "Events/ItemBreakEvent.h"

namespace drft::system
{
	class ItemDurabilitySystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		void onItemBreakEvent(events::ItemBreakEvent& ev);
	};
}

