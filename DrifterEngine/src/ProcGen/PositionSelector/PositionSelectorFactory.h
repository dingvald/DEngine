#pragma once
#include <Factory/Factory.h>
#include <ProcGen/PositionSelector/IPositionSelector.h>

using PositionSelectorFactory = Factory<IPositionSelector>;

void bindPositionSelectors(PositionSelectorFactory& selectorFactory);
