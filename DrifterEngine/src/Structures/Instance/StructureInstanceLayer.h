#pragma once
#include <ProcGen/SlotPositionList.h>

class StructureInstanceLayer
{
public:
	const SlotPositionMap& getEntitySlots() const;
private:
	SlotPositionMap _slots;
};