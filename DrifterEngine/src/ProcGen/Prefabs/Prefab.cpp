#include <pch.h>
#include "Prefab.h"
#include <ProcGen/GenerationContext.h>
#include <ProcGen/Layers/CanvasLayer.h>

using namespace entt::literals;

const int PREFAB_PRIORITY = 1000;

void Prefab::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("layers"))
	{
		auto layers = json["layers"].GetArray();
		_layers.reserve(layers.Size());
		for (auto&& layer : layers)
		{
			SlotPositionList newList;
			if (layer.HasMember("entities"))
			{
				auto entities = layer["entities"].GetArray();
				newList.reserve(entities.Size());
				for (auto&& entity : entities)
				{
					SlotPositionPair newPair;

					newPair.slot = entt::hashed_string{ entity["id"].GetString() };
					newPair.position.x = entity["x"].GetInt();
					newPair.position.y = entity["y"].GetInt();

					_area.position.x = std::min(_area.position.x, newPair.position.x);
					_area.position.y = std::min(_area.position.x, newPair.position.y);
					_area.size.x = std::max(_area.size.x, newPair.position.x - _area.position.x);
					_area.size.y = std::max(_area.size.y, newPair.position.y - _area.position.y);

					newList.emplace_back(std::move(newPair));
				}
			}
			_layers.emplace_back(std::move(newList));
		}
	}
}

SlotPositionList Prefab::generate(sf::Vector3i position, const GenerationContext& context) const
{
	SlotPositionList result;
	for (int i = 0; i < _layers.size(); i++)
	{
		for (auto&& [slot, pos, priority] : _layers.at(i))
		{
			sf::Vector3i finalPos = pos + position;
			const int finalPriority = priority == CanvasLayer::UninitializedPriority ? PREFAB_PRIORITY : priority;
			result.emplace_back(slot, finalPos, finalPriority);
		}
	}
	return result;
}

const sf::IntRect& Prefab::getArea() const
{
	return _area;
}
