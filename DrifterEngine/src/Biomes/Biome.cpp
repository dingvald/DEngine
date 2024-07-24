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
	if (json.HasMember("entity_slots"))
	{
		for (auto& slot : json["entity_slots"].GetObject())
		{
			EntitySlot newSlot;
			auto slotName = slot.name.GetString();
			
			if (slot.value.HasMember("probability"))
			{
				newSlot.probability = slot.value["probability"].GetFloat();
			}
			if (slot.value.HasMember("probability_multipliers"))
			{
				for (auto&& multVal : slot.value["probability_multipliers"].GetArray())
				{
					auto multObj = multVal.GetObject();
					const auto type = multObj["type"].GetString();
					auto multiplier = MuliplierFactory::build(type);
					multiplier->createFromJSON(multObj["params"]);

					newSlot.multipliers.emplace_back(std::move(multiplier));
				}
			}

			_entitySlots.emplace(entt::hashed_string{ slotName }, std::move(newSlot));
		}
	}
	if (json.HasMember("entity_packs"))
	{
		for (auto& pack : json["entity_packs"].GetObject())
		{
			auto packName = pack.name.GetString();
			std::vector<EntityWeight> newPack;
			auto packArray = pack.value.GetArray();
			for (auto&& val : packArray)
			{
				std::string entityName = val.GetArray()[0].GetString();
				int weight = val.GetArray()[1].GetInt();
				newPack.emplace_back(std::make_pair(entityName, weight));
			}
			_entityPacks.emplace(entt::hashed_string{ packName }, newPack);
		}
	}
	if (json.HasMember("structures"))
	{
		for (auto& structure : json["structures"].GetObject())
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

bool Biome::satisfiesClimate(const std::string& climateName, float value) const
{
	if (!_ranges.contains(climateName)) return false;
	const auto& range = _ranges.at(climateName);
	return (range.isInfinite() || range.isValueWithin(value));
}

float Biome::getDeviationFromClimate(const std::string& climateName, float value) const
{
	if (!_ranges.contains(climateName)) return 0.0f;
	const auto& range = _ranges.at(climateName);
	return range.distance(value);
}

BiomeIcon Biome::getIcon() const
{
	return _icon;
}

const std::string& Biome::getName() const
{
	return _name;
}

const std::unordered_map<entt::id_type, Biome::EntitySlot>& Biome::getEntitySlots() const
{
	return _entitySlots;
}

const std::unordered_map<entt::id_type, std::vector<Biome::EntityWeight>>& Biome::getEntityPacks() const
{
	return _entityPacks;
}

const std::vector<Biome::StructureProbabilityPair>& Biome::getStructureProbabilities() const
{
	return _structures;
}