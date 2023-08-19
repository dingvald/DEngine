#pragma once
#include <string>
#include <SFML/Graphics.hpp>

namespace drft::events
{
	struct SendFloatingMessageEvent
	{
		std::string message = "INVALID";
		sf::Color color = sf::Color::Magenta;
		sf::Vector2i position;
		sf::Vector2f velocity;
		bool isScreenSpace = true;
		int ttl = 10;
	};
}