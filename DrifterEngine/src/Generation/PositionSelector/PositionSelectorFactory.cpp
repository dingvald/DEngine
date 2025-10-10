#include "pch.h"
#include "PositionSelectorFactory.h"
#include "AnchorPositionSelector.h"
#include "PositionSelectorIds.h"

void bindPositionSelectors(PositionSelectorFactory& selectorFactory)
{
    // Example registration:
    // selectorFactory.registerType<RandomPositionSelector>(PositionSelectorId::Random);
    // selectorFactory.registerType<CenterPositionSelector>(PositionSelectorId::Center);
    selectorFactory.registerType<AnchorPositionSelector>(PositionSelectorId::Anchor);
    // Add more registrations as needed
}
