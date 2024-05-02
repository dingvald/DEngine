#include "pch.h"
#include "Biome.h"

Biome::Biome(std::string name)
	:_name(name)
{}

void Biome::createFromJSON(const rapidjson::Value& json)
{
	if (json.HasMember("Icon"))
	{
		_icon.sprite = json["Icon"]["Sprite"].GetInt();
		_icon.color.r = json["Icon"]["Color"].GetArray()[0].GetInt();
		_icon.color.g = json["Icon"]["Color"].GetArray()[1].GetInt();
		_icon.color.b = json["Icon"]["Color"].GetArray()[2].GetInt();
	}
	if (json.HasMember("Climate"))
	{
		for (auto& climateRange : json["Climate"].GetObject())
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
	if (json.HasMember("Entities"))
	{
		for (auto& category : json["Entities"].GetObject())
		{
			for (auto& entity : category.value.GetObject())
			{
				if (entity.value.HasMember("Algorithm")
					&& entity.value.HasMember("Parameters"))
				{
					std::string algoName = entity.value["Algorithm"].GetString();

					SpawningAlgorithm spawningAlgorithm;
					spawningAlgorithm.name = algoName;
					for (auto& param : entity.value["Parameters"].GetObject())
					{
						if (param.value.IsString())
						{
							spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetString();
						}
						else if (param.value.IsBool())
						{
							spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetBool();
						}
						else
						{
							spawningAlgorithm.parameters[param.name.GetString()] = param.value.GetFloat();
						}
					}
					_entityCategories[category.name.GetString()][entity.name.GetString()] = spawningAlgorithm;
				}
				else
				{
					throw std::exception("Parsed entity does not have a placement algorithm");
				}
			}
		}
	}
	if (json.HasMember("Structures"))
	{
		for (auto& structure : json["Structures"].GetObject())
		{
			_structures.emplace_back(structure.name.GetString(), structure.value.GetFloat());
		}
	}
}

bool Biome::containsClimateRange(const std::string& name) const
{
	return _ranges.contains(name);
}

drft::math::Range<float> Biome::getClimateRange(const std::string& name) const
{
	return _ranges.at(name);
}

BiomeIcon Biome::getIcon() const
{
	return _icon;
}

const Biome::EntityCategories& Biome::getEntitySpawningAlgorithms() const
{
	return _entityCategories;
}

const std::vector<Biome::StructureProbabilityPair>& Biome::getStructureProbabilities() const
{
	return _structures;
}
