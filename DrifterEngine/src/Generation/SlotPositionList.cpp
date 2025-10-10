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

void mergeSlotPositionMaps(SlotPositionMap& map, const SlotPositionMap& other, std::function<bool(sf::Vector3i)> pred)
{
	for (auto&& [pos, slotPriority] : other)
	{
		if (!pred(pos)) continue;

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

void mergeSlotListIntoMap(SlotPositionMap& map, const SlotPositionList& list)
{
	for (auto&& [slot, position, priority] : list)
	{
		if (auto it = map.find(position); it != map.end())
		{
			if (priority > it->second.priority)
			{
				it->second.slot = slot;
				it->second.priority = priority;
			}
		}
		else
		{
			map.emplace(position, SlotPriority{ slot, priority });
		}
	}
}
