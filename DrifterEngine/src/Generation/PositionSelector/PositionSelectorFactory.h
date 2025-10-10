#pragma once
#include <Factory/Factory.h>
#include <Generation/PositionSelector/IPositionSelector.h>

using PositionSelectorFactory = Factory<IPositionSelector>;

void bindPositionSelectors(PositionSelectorFactory& selectorFactory);
