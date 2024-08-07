#pragma once
#include <SFML/System/Vector2.hpp>
#include <EnTT/entity/fwd.hpp>

struct GenerationLayerDependency
{
	entt::id_type layerID;
	sf::Vector2i padding;
};