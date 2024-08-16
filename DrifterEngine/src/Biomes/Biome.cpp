#include "pch.h"
#include "Biome.h"
#include "Factory/Factory.h"

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
}

float Biome::distanceFromClimate(const std::unordered_map<std::string, float>& values) const
{
	float sum = 0.0;
	for (auto&& [name, val] : values)
	{
		if (!_ranges.contains(name)) continue;
		sum += _ranges.at(name).distance(val);
	}
	return sum;
}

const std::unordered_map<std::string, drft::math::Range<float>>& Biome::getClimateRanges() const
{
	return _ranges;
}

BiomeIcon Biome::getIcon() const
{
	return _icon;
}

const std::string& Biome::getName() const
{
	return _name;
}