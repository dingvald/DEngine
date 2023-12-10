#include <pch.h>
#include "Algorithms.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Factory/EntityFactory.h"

void drft::gen::place(const std::string& name, sf::Vector2i origin, sf::Vector2i position, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	auto worldPos = origin + position;
	factory.build(name, reg)
		.patch<component::Position>([worldPos](auto& pos)
			{
				pos.position = worldPos;
			});
}

void drft::gen::place(const std::string& name, sf::Vector2i tileOrigin, const std::vector<sf::Vector2i>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		auto position = tileOrigin + pos;
		factory.build(name, reg)
			.patch<component::Position>([position](auto& pos)
				{
					pos.position = position;
				});
	}
}

void drft::gen::fastFill(const std::string& name, sf::Vector2i tileOrigin, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (int y = 0; y < spatial::CHUNK_HEIGHT; ++y)
	{
		for (int x = 0; x < spatial::CHUNK_WIDTH; ++x)
		{
			const auto position = tileOrigin + sf::Vector2i(x,y);
			factory.build(name, reg)
				.patch<component::Position>([position](auto& pos)
					{
						pos.position = position;
					});
		}
	}
}


