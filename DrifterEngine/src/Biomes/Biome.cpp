#include "pch.h"
#include "Biome.h"
#include "Factory/Factory.h"
#include <Utility/StandardErrorLogger.h>


namespace evaluation_functions
{
	static bool isGreater(float val, drft::math::Range<float> range)
	{
		return val > range.getMax();
	}

	static bool isLess(float val, drft::math::Range<float> range)
	{
		return val < range.getMin();
	}

	static bool isInside(float val, drft::math::Range<float> range)
	{
		return range.isValueWithin(val);
	}

	static bool isOutside(float val, drft::math::Range<float> range)
	{
		return !range.isValueWithin(val);
	}
}

Biome::Biome(std::string name)
	:_name(name)
{}

void Biome::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("icon"))
	{
		_icon.texture = entt::hashed_string(json["icon"]["texture"].GetString());
		_icon.uvSize.x = json["icon"]["uv_size"].GetArray()[0].GetFloat();
		_icon.uvSize.y = json["icon"]["uv_size"].GetArray()[1].GetFloat();
		_icon.uvCoords.x = json["icon"]["uv_coords"].GetArray()[0].GetFloat();
		_icon.uvCoords.y = json["icon"]["uv_coords"].GetArray()[1].GetFloat();
		_icon.color.r = json["icon"]["color"].GetArray()[0].GetInt();
		_icon.color.g = json["icon"]["color"].GetArray()[1].GetInt();
		_icon.color.b = json["icon"]["color"].GetArray()[2].GetInt();
	}
	if (json.HasMember("climate"))
	{
		for (auto& climateRange : json["climate"].GetObject())
		{
			auto name = climateRange.name.GetString();
			auto range = climateRange.value.GetArray();
			if (range[0].IsString())
			{
				if (std::string(range[0].GetString()).compare("Any") == 0)
				{
					_ranges[name].setInfinite();
				}
				else
				{
					throw std::invalid_argument("Unexpected parsed value.");
				}
			}
			else
			{
				_ranges[name].setMin(range[0].GetFloat());
				_ranges[name].setMax(range[1].GetFloat());
			}
		}
	}
	if (json.HasMember("entity_slots"))
	{
		for (auto&& [name, value] : json["entity_slots"].GetObject())
		{
			SlotDependency newDependency;
			newDependency.layerID = entt::hashed_string{ value["layer_id"].GetString() };
			auto thresholdExpression = value["threshold"].GetArray();
			std::string conditional = thresholdExpression[0].GetString();

			if (thresholdExpression.Size() == 2)
			{
				float range = thresholdExpression[1].GetFloat();
				newDependency.range.setMin(range);
				newDependency.range.setMax(range);
				if (conditional == ">")
				{
					newDependency.satisfiesValue = evaluation_functions::isGreater;
				}
				else if (conditional == "<")
				{
					newDependency.satisfiesValue = evaluation_functions::isLess;
				}
			}
			else if (thresholdExpression.Size() == 3)
			{
				float range_min = thresholdExpression[1].GetFloat();
				float range_max = thresholdExpression[2].GetFloat();
				newDependency.range.setMin(range_min);
				newDependency.range.setMax(range_max);
				if (conditional == ">")
				{
					newDependency.satisfiesValue = evaluation_functions::isOutside;
				}
				else if (conditional == "<")
				{
					newDependency.satisfiesValue = evaluation_functions::isInside;
				}
			}
			else
			{
				error_logger << "Invalid error expression with name " << name.GetString() << std::endl;
			}

			_entitySlotDependencies.emplace(entt::hashed_string{ name.GetString() }, std::move(newDependency));
		}
	}
	if (json.HasMember("entity_packs"))
	{
		for (auto&& [name, value] : json["entity_packs"].GetObject())
		{
			EntityPack entityPack;
			for (auto&& val : value.GetArray())
			{
				auto pair = val.GetArray();
				std::string entityName = pair[0].GetString();
				int entityWeight = pair[1].GetInt();
				entityPack.emplace_back(std::make_pair(std::move(entityName), entityWeight));
			}
			_entityPacks.emplace(entt::hashed_string{ name.GetString() }, std::move(entityPack));
		}
	}
}

bool Biome::satisfiesClimate(const std::unordered_map<std::string, float>& values) const
{
	for (auto&& [name, val] : values)
	{
		if (!_ranges.contains(name)) continue;
		if (!_ranges.at(name).isValueWithin(val)) return false;
	}
	return true;
}

const std::unordered_map<std::string, drft::math::Range<float>>& Biome::getClimateRanges() const
{
	return _ranges;
}

const std::unordered_map<entt::id_type, SlotDependency>& Biome::getSlotDependencies() const
{
	return _entitySlotDependencies;
}

const EntityPack& Biome::getEntityPack(entt::id_type slotID) const
{
	return _entityPacks.at(slotID);
}

BiomeIcon Biome::getIcon() const
{
	return _icon;
}

const std::string& Biome::getName() const
{
	return _name;
}