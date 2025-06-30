#pragma once
#include <ProcGen/Features/IFeature.h>
#include <Utility/Math.h>
#include <SFML/System/Vector2.hpp>

class ChambersFeature : public IFeature
{
public:
    void createFromJson(const rapidjson::Value& json) override;
    TaggedPositions generateTags(sf::Vector3i position, const GenerationContext& context) const override;

private:
    TagList _wallTags;
    TagList _centerTags;
    TagList _floorTags;
    TagList _hallwayTags;
    TagList _doorTags;
    drft::math::Range<int> _roomCount;
    drft::math::Range<int> _roomSize;
    drft::math::Range<int> _entrances;
    sf::Vector2i _maxArea = {0, 0};
};
   