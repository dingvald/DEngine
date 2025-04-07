#include "pch.h"
#include "Biome.h"
#include "Factory/Factory.h"

#include <Utility/Math.h>
#include <Utility/StandardLogger.h>
#include <Utility/ContainerHelpers.h>


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

void SlotDependency::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("threshold"))
	{
		auto thresholdExpression = json["threshold"].GetArray();
		std::string conditional = thresholdExpression[0].GetString();

		if (thresholdExpression.Size() == 2)
		{
			float rangeJson = thresholdExpression[1].GetFloat();
			range.setMin(rangeJson);
			range.setMax(rangeJson);
			if (conditional == ">")
			{
				satisfiesValue = evaluation_functions::isGreater;
			}
			else if (conditional == "<")
			{
				satisfiesValue = evaluation_functions::isLess;
			}
		}
		else if (thresholdExpression.Size() == 3)
		{
			float range_min = thresholdExpression[1].GetFloat();
			float range_max = thresholdExpression[2].GetFloat();
			range.setMin(range_min);
			range.setMax(range_max);
			if (conditional == ">")
			{
				satisfiesValue = evaluation_functions::isOutside;
			}
			else if (conditional == "<")
			{
				satisfiesValue = evaluation_functions::isInside;
			}
		}
	}
}

void SlotDeterminer::createFromJson(const rapidjson::Value& json)
{
	if (json.HasMember("layers"))
	{
		for (auto&& [name, value] : json["layers"].GetObject())
		{
			SlotDependency newDependency;
			newDependency.createFromJson(value);
			dependencies.emplace(entt::hashed_string{ name.GetString() }, std::move(newDependency));
		}
	}
	if (json.HasMember("expression"))
	{
		expression = drft::util::BooleanStringExpression{ json["expression"].GetString() };
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
		setBaseTileColor(_icon.color);
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
					_ranges[entt::hashed_string{ name }].setInfinite();
				}
				else
				{
					throw std::invalid_argument("Unexpected parsed value.");
				}
			}
			else
			{
				_ranges[entt::hashed_string{ name }].setMin(range[0].GetFloat());
				_ranges[entt::hashed_string{ name }].setMax(range[1].GetFloat());
			}
		}
	}
	if (json.HasMember("entity_slots"))
	{
		for (auto&& [name, value] : json["entity_slots"].GetObject())
		{
			SlotDeterminer newDeterminer;
			newDeterminer.createFromJson(value);
			_entitySlotDeterminers.emplace(entt::hashed_string{ name.GetString() }, std::move(newDeterminer));
		}
	}
}

bool Biome::satisfiesClimate(const std::unordered_map<entt::id_type, float>& values) const
{
	for (auto&& [id, val] : values)
	{
		if (!_ranges.contains(id)) continue;
		if (!_ranges.at(id).isValueWithin(val)) return false;
	}
	return true;
}

float Biome::closenessToClimate(const std::unordered_map<entt::id_type, float>& values) const
{
	float result = 0.f;
	for (auto&& [id, val] : values)
	{
		if (!_ranges.contains(id))
		{
			result += 1.0f;
			continue;
		}
		result += _ranges.at(id).distance(val);
	}
	return result;
}

std::vector<entt::id_type> Biome::getClimateDependencyIds() const
{
	return drft::util::extractKeys(_ranges);
}

std::vector<entt::id_type> Biome::getEntitySlotDependencyIds() const
{
	std::vector<entt::id_type> result;
	for (auto&& [slotID, determiner] : _entitySlotDeterminers)
	{
		auto ids = drft::util::extractKeys(determiner.dependencies);
		result.insert(result.end(), std::make_move_iterator(ids.begin()), std::make_move_iterator(ids.end()));
	}
	return result;
}

std::vector<entt::id_type> Biome::determineValidSlots(const std::unordered_map<entt::id_type, float>& dependencyValues) const
{
	std::vector<entt::id_type> result;
	for (auto&& [slotId, slotDeterminer] : _entitySlotDeterminers)
	{
		TokenValues values;
		for (auto&& [layerID, slotDependency] : slotDeterminer.dependencies)
		{
			if (!dependencyValues.contains(layerID)) continue;
			values.emplace(layerID, slotDependency.satisfiesValue(dependencyValues.at(layerID), slotDependency.range));
		}
		if (slotDeterminer.expression.evaluate(values))
		{
			result.emplace_back(slotId);
		}
	}
	return result;
}

BiomeIcon Biome::getIcon() const
{
	return _icon;
}

sf::Color Biome::getBaseTileColor() const
{
	return _baseTileColor;
}

const std::string& Biome::getName() const
{
	return _name;
}

void Biome::setBaseTileColor(sf::Color iconColor)
{
	auto max_val = std::max(iconColor.r, std::max(iconColor.g, iconColor.b));
	sf::Color intermediate;
	intermediate.r = static_cast<std::uint8_t>(drft::math::remap(0, max_val, 0, 255, iconColor.r));
	intermediate.g = static_cast<std::uint8_t>(drft::math::remap(0, max_val, 0, 255, iconColor.g));
	intermediate.b = static_cast<std::uint8_t>(drft::math::remap(0, max_val, 0, 255, iconColor.b));

	_baseTileColor.r = static_cast<std::uint8_t>(drft::math::remap(0, 255, 0, 40, intermediate.r));
	_baseTileColor.g = static_cast<std::uint8_t>(drft::math::remap(0, 255, 0, 40, intermediate.g));
	_baseTileColor.b = static_cast<std::uint8_t>(drft::math::remap(0, 255, 0, 40, intermediate.b));
}


