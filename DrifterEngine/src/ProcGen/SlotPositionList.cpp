#include "pch.h"
#include "SlotPositionList.h"
#include <Utility/stdHashing.h>

void mergeSlotPositionMaps(SlotPositionMap& map, const SlotPositionMap& other)
{
	for (auto&& [pos, slotPriority] : other)
	{
		if (!map.contains(pos))
		{
			map.emplace(pos, SlotPriority{ slotPriority.slot, slotPriority.priority });
			continue;
		}

		auto& slot = map.at(pos);
		if (slot.priority > slotPriority.priority) continue;

		slot.slot = slotPriority.slot;
		slot.priority = slotPriority.priority;
	}
}