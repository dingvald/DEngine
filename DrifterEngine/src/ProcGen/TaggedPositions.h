#pragma once
#include <vector>
#include<EnTT/core/fwd.hpp>
#include <SFML/System/Vector3.hpp>
#include <unordered_map>


using PositionList = std::vector<sf::Vector3i>;
using TaggedPositions = std::unordered_map<entt::id_type, PositionList>;