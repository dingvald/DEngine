#pragma once
#include <ProcGen/Features/IFeature.h>
#include <Utility/Math.h>

class ChambersFeature : public IFeature
{
public:
    void createFromJson(const rapidjson::Value& json) override;
    TaggedPositions generateTags(sf::Vector3i position, const GenerationContext& context) const override;

private:
    TagList _wallTags;
    TagList _centerTags;
    TagList _floorTags;
    drft::math::Range<int> _roomCount;
    drft::math::Range<int> _roomSize;
};
   