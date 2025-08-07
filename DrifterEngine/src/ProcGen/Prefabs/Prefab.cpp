#include <pch.h>
#include "Prefab.h"
#include <ProcGen/GenerationContext.h>
#include <Spatial/Helpers.h>
#include <Utility/StandardLogger.h>

using namespace entt::literals;

const int PREFAB_PRIORITY = 1000;

void Prefab::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("layers"))
	{
		// Fill a temporary map causing lower layers to be overwritten by higher layers
		std::unordered_map<sf::Vector2i, entt::id_type> tempPositionMap;
		for (auto&& layer : json["layers"].GetArray())
		{
			if (layer.HasMember("entities"))
			{
				auto entities = layer["entities"].GetArray();
				for (auto&& entity : entities)
				{
					sf::Vector2i position;

					entt::id_type slot = entt::hashed_string{ entity["id"].GetString() };
					position.x = entity["x"].GetInt();
					position.y = entity["y"].GetInt();

					tempPositionMap[position] = slot;
				}
			}
		}

		// Create the list and set the area
		for (auto&& [position, slot] : tempPositionMap)
		{
			_area.position.x = std::min(_area.position.x, position.x);
			_area.position.y = std::min(_area.position.x, position.y);
			_area.size.x = std::max(_area.size.x, position.x - _area.position.x);
			_area.size.y = std::max(_area.size.y, position.y - _area.position.y);

			_entitySlots.emplace_back(slot, drft::spatial::vec3FromPlanar(position), PREFAB_PRIORITY);
		}
	}
	else
	{
		LOG_ERROR("Prefab file is missing the 'layers' member");
	}
}

SlotPositionList Prefab::generate(sf::Vector3i position, const GenerationContext& context) const
{
	SlotPositionList result;
	result.reserve(_entitySlots.size());
	for (auto&& [slot, pos, priority] : _entitySlots)
	{
		result.emplace_back(slot, pos + position, priority);
	}
	return result;
}

const sf::IntRect& Prefab::getArea() const
{
	return _area;
}
