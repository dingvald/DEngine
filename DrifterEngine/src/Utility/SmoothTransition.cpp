#include "pch.h"
#include "SmoothTransition.h"

drft::util::SmoothTransition::SmoothTransition(float startVal, float endVal, float startOfRange, float endOfRange)
    : _startVal(startVal)
    , _endVal(endVal)
    , _startOfRange(startOfRange)
    , _endOfRange(endOfRange)
{}

float drft::util::SmoothTransition::compute(float currentRange) const
{
    float range = std::clamp(currentRange, _startOfRange, _endOfRange);
    float deltaVal = _endVal - _startVal;
    float deltaRange = _endOfRange - _startOfRange;
    float inc = deltaVal / deltaRange;

    return _startVal + ((range - _startOfRange)*inc);
}

drft::util::SmoothColorTransition::SmoothColorTransition(sf::Color start, sf::Color end, float startOfRange, float endOfRange)
    : _r(start.r, end.r, startOfRange, endOfRange)
    , _g(start.g, end.g, startOfRange, endOfRange)
    , _b(start.b, end.b, startOfRange, endOfRange)
{}

sf::Color drft::util::SmoothColorTransition::compute(float currentRange) const
{
    sf::Color result;
    result.r = _r.compute(currentRange);
    result.g = _g.compute(currentRange);
    result.b = _b.compute(currentRange);

    return result;
}
