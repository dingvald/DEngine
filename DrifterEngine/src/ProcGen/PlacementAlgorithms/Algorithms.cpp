#include <pch.h>
#include "Algorithms.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Factory/EntityFactory.h"

void drft::gen::place(const std::string& name, sf::Vector2i tileOrigin, const std::vector<sf::Vector2i>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();

	for (auto pos : positions)
	{
		auto position = spatial::toWorldSpace(tileOrigin + pos);
		factory.build(name, reg)
			.patch<component::Position>([position](auto& pos)
				{
					pos.position = position;
				});
	}
}
