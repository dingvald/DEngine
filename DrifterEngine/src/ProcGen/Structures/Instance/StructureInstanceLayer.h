#pragma once
#include <ProcGen/SlotPositionList.h>

class StructureInstanceLayer
{
public:
	StructureInstanceLayer(SlotPositionMap&& slots);
	const SlotPositionMap& getEntitySlots() const;
private:
	SlotPositionMap _slots;
};