#include "pch.h"
#include "Vector3Utils.h"
#include <Utility/Math.h>

bool drft::Vector3Utils::isNear(sf::Vector3f v1, sf::Vector3f v2, float epsilon)
{
    const float dx = std::abs(v1.x - v2.x);
    const float dy = std::abs(v1.y - v2.y);
    const float dz = std::abs(v1.z - v2.z);

    return dx < epsilon && dy < epsilon && dz < epsilon;
}

sf::Vector3f drft::Vector3Utils::normalize(sf::Vector3f v)
{
    const float mag = magnitude(v);
    return sf::Vector3f{v.x/mag, v.y/mag, v.z/mag};
}

float drft::Vector3Utils::magnitude(sf::Vector3f v)
{
    return std::sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

sf::Vector3f drft::Vector3Utils::floor(const sf::Vector3f& v)
{
    sf::Vector3f result;
    result.x = std::floor(v.x);
    result.y = std::floor(v.y);
    result.z = std::floor(v.z);
    return result;
}

sf::Vector3f drft::Vector3Utils::floorToMultiple(const sf::Vector3f& v, float multiple)
{
    sf::Vector3f result;
    result.x = math::floorToMultiple(v.x, multiple);
    result.y = math::floorToMultiple(v.y, multiple);
    result.z = math::floorToMultiple(v.z, multiple);
    return result;
}

sf::Vector2f drft::Vector3Utils::floor(const sf::Vector2f v)
{
    sf::Vector2f result;
    result.x = std::floor(v.x);
    result.y = std::floor(v.y);
    return result;
}
