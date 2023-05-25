#pragma once
#include "EnTT/entt.h"
#include "SFML/Graphics.hpp"

namespace drft::events
{
	struct EnterTileEvent
	{
		entt::entity entity;
		sf::Vector2i tilePosition;
	};
}