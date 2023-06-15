#pragma once

struct TargetingParameters
{
	sf::Vector2f origin;
	std::vector<sf::Vector2i> targetArea;
	bool shouldDrawLine = false;
	std::function<void(entt::entity, sf::Vector2i)> selectTarget;
};

