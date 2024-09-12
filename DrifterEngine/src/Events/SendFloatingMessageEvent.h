#pragma once
#include <string>
#include <SFML/Graphics.hpp>

namespace drft::events
{
	struct SendFloatingMessageEvent
	{
		std::string message = "INVALID";
		sf::Color color = sf::Color::Magenta;
		entt::entity tracksEntity = entt::null;
		sf::Vector2f position;
		sf::Vector2f velocity;
		bool fades = false;
		bool isScreenSpace = true;
		int ttl = 10;
	};
}