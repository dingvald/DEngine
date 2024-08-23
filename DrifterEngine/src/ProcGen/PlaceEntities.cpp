#include "pch.h"
#include "PlaceEntities.h"
#include "Factory/EntityFactory.h"
#include "Components/PositionComponent.h"

entt::handle drft::gen::placeSingle(const std::string& name, sf::Vector2i position, entt::registry& registry, const EntityFactory& factory)
{
	auto handle = factory.build(name, registry);
	handle.patch<PositionComponent>([position](auto& pos)
			{
				pos.position = position;
			});
	return handle;
}

void drft::gen::placeManyConditional(const std::string& name, sf::IntRect area, entt::registry& registry, std::function<bool(sf::Vector2i)> predicate)
{
	const auto& factory = registry.ctx().get<const EntityFactory&>();
	const sf::Vector2i origin = { area.left, area.top };
	for (int y = area.top; y < area.top + area.height; ++y)
	{
		for (int x = area.left; x < area.left + area.width; ++x)
		{
			if (!predicate({ x, y })) continue;

			auto position = origin + sf::Vector2i{x, y};
			placeSingle(name, position, registry, factory);
		}
	}
}

void drft::gen::placeMany(const std::string& name, sf::IntRect area, entt::registry& registry)
{
	const auto& factory = registry.ctx().get<const EntityFactory&>();
	const sf::Vector2i origin = { area.left, area.top };
	for (int y = area.top; y < area.top + area.height; ++y)
	{
		for (int x = area.left; x < area.left + area.width; ++x)
		{
			placeSingle(name, {x, y}, registry, factory);
		}
	}
}

void drft::gen::placeMany(const std::string& name, const std::vector<sf::Vector2i>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		placeSingle(name, pos, reg, factory);
	}
}

void drft::gen::placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		placeSingle(name, origin + pos, reg, factory);
	}
}

void drft::gen::placeMany(const std::string& name, sf::Vector2i origin, const std::vector<sf::Vector2i>& positions, entt::registry& reg, std::function<void(entt::handle, sf::Vector2i)> onPlaceFunc)
{
	const auto& factory = reg.ctx().get<const EntityFactory&>();
	for (auto&& pos : positions)
	{
		auto handle = placeSingle(name, origin + pos, reg, factory);
		onPlaceFunc(handle, pos);
	}
}
