#include <pch.h>
#include "DepthLayer.h"

namespace drft
{
    void DepthLayer::createFromJson(const rapidjson::Value&)
    {
        // No need
    }

    double DepthLayer::getValueAt(sf::Vector3i tilePosition)
    {
        return tilePosition.z;
    }
}
