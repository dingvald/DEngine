#include "pch.h"
#include "FastFill.h"
#include "Factory/EntityFactory.h"
#include "Spatial/Conversions.h"
#include "Components/Components.h"

void drft::gen::fastFill(const std::string& name, sf::Vector2i origin, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (int y = 0; y < spatial::CHUNK_HEIGHT; ++y)
	{
		for (int x = 0; x < spatial::CHUNK_WIDTH; ++x)
		{
			const auto position = origin + sf::Vector2i(x, y);
			factory.build(name, reg)
				.patch<component::Position>([position](auto& pos)
					{
						pos.position = position;
					});
		}
	}
}
