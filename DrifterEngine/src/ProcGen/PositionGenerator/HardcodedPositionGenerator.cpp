#include "pch.h"
#include "HardcodedPositionGenerator.h"
#include <Utility/StandardLogger.h>

void HardcodedPositionGenerator::createFromJson(const rapidjson::Value& json)
{
    if (json.HasMember("location"))
    {
        auto locationArray = json["location"].GetArray();
        _location.x = locationArray[0].GetInt();
        _location.y = locationArray[1].GetInt();
        _location.z = locationArray[2].GetInt();
    }
    else
    {
        LOG_ERROR("'hardcoded' condition requires a 'location' member");
    }
}

IPositionGenerator::PositionSet HardcodedPositionGenerator::generatePositions(const PositionGeneratorContext& context) const
{
    return { _location };
}
