#include "pch.h"
#include "PlaceEntities.h"
#include "Factory/EntityFactory.h"
#include "Components/PositionComponent.h"

void drft::gen::placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		auto position = origin + pos;
		factory.build(name, reg)
			.patch<PositionComponent>([position](auto& pos)
				{
					pos.position = position;
				});
	}
}

void drft::gen::placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg, std::function<void(entt::handle, sf::Vector2i)> onPlaceFunc)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		auto position = origin + pos;
		auto handle = factory.build(name, reg);
		handle.patch<PositionComponent>([position](auto& pos)
			{
				pos.position = position;
			});
		onPlaceFunc(handle, pos);
	}
}
