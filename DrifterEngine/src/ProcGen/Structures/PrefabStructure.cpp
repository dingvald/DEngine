#include <pch.h>
#include "PrefabStructure.h"
#include <ProcGen/GenerationContext.h>

using namespace entt::literals;

void PrefabStructure::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("reference_depth"))
	{
		_volume.min.z = json["reference_depth"].GetInt();
	}
	else
	{
		_volume.min.z = 0;
	}

	if (json.HasMember("layers"))
	{
		auto layers = json["layers"].GetArray();
		_volume.max.z = _volume.min.z + layers.Size() - 1;
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

					_volume.min.x = std::min(_volume.min.x, newPair.position.x);
					_volume.min.y = std::min(_volume.min.y, newPair.position.y);
					_volume.max.x = std::max(_volume.max.x, newPair.position.x);
					_volume.max.y = std::max(_volume.max.y, newPair.position.y);

					newList.emplace_back(std::move(newPair));
				}
			}
			_layers.emplace_back(std::move(newList));
		}
	}
}

TaggedPositions PrefabStructure::generate(sf::Vector3i position, const GenerationContext& context) const
{
	TaggedPositions result;
	for (int i = 0; i < _layers.size(); i++)
	{
		const int localZ = i + _volume.min.z;
		for (auto&& [slot, pos] : _layers.at(i))
		{
			sf::Vector3i finalPos = pos + position;
			finalPos.z += localZ;
			result[slot].push_back(finalPos);
		}
	}
	return result;
}

const drft::spatial::AABB<int>& PrefabStructure::getVolume() const
{
	return _volume;
}
