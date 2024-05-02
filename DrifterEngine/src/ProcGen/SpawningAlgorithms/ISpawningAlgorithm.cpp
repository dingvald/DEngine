#include "pch.h"
#include "ISpawningAlgorithm.h"
#include "Factory/EntityFactory.h"
#include "Components/Components.h"

void drft::gen::place(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		auto position = origin + pos;
		factory.build(name, reg)
			.patch<component::Position>([position](auto& pos)
				{
					pos.position = position;
				});
	}
}