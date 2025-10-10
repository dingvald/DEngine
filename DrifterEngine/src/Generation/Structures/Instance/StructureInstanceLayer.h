#pragma once
#include <Generation/SlotPositionList.h>

class StructureInstanceLayer
{
public:
	StructureInstanceLayer(SlotPositionMap&& slots);
	const SlotPositionMap& getEntitySlots() const;
private:
	SlotPositionMap _slots;
};