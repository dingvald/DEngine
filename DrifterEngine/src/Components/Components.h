#pragma once
#include "Actions/Action.h"
#include "Actions/ActionBuffer.h"

namespace component
{
	struct Prototype 
	{};

	struct Info
	{
		std::string prototype = { "" };
		std::string name = { "" };
		std::string extendedName = { "" };
		std::string description = { "" };
	};

	// Position in world space.
	struct Position
	{
		sf::Vector2f position = { 0,0 };
		int depth = { 0 };
	};

	// Sprite and color.
	struct Render
	{
		unsigned int sprite = { 0 };
		sf::Color color = { sf::Color::Magenta };
	};

	struct Camera
	{
		sf::FloatRect viewport = { 0,0,0,0 };
		entt::entity target = { entt::null };
	};

	struct Actor
	{
		int ap = { 0 };
		float moveSpeed = { 1.0f };
		float actSpeed = { 1.0f };
	};

	struct Health
	{
		int max = 10;
		int current = 10;
	};

	struct Attacker
	{
		int baseDamage = 1;
	};

	// Represents a player
	struct Player
	{
		drft::action::ActionBuffer actionBuffer;
	};

	// Represents an AI
	struct AI
	{
		std::string protocol = { "random" };
	};

	struct Damage
	{
		int amount = 0;
	};

} // namespace component