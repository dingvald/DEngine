#pragma once
#include <string>
#include <SFML/Graphics.hpp>

namespace drft::events
{
	struct SendFloatingMessageEvent
	{
		std::string message = "INVALID";
		sf::Color color = sf::Color::Magenta;
		sf::Vector2f position;
		bool isScreenSpace = true;
		int ttl = 10;
	};
}