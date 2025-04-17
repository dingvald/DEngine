#include "pch.h"
#include "Biome.h"

#include <Utility/Math.h>
#include <Utility/ContainerHelpers.h>


Biome::Biome(std::string name)
	:_name(name)
{}

void Biome::createFromJson(const rapidjson::Value& json)
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
		auto& val = json["climate"];
		_climateDeterminer.createFromJson(val);
	}
	if (json.HasMember("entity_slots"))
	{
		for (auto&& [name, value] : json["entity_slots"].GetObject())
		{
			SlotDeterminer newDeterminer;
			newDeterminer.createFromJson(value);
			_entitySlotDeterminers.emplace_back(entt::hashed_string{ name.GetString() }, std::move(newDeterminer));
		}
	}
	if (json.HasMember("features"))
	{
		for (auto&& [name, value] : json["features"].GetObject())
		{
			SlotDeterminer newDeterminer;
			newDeterminer.createFromJson(value);
			_featureDeterminers.emplace_back(entt::hashed_string{ name.GetString() }, std::move(newDeterminer));
		}
	}
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

bool Biome::satisfiesClimate(const DependencyValues& dependencyValues) const
{
	TokenValues values;
	for (auto&& [layerID, slotDependency] : _climateDeterminer.dependencies)
	{
		if (!dependencyValues.contains(layerID)) continue;
		values.emplace(layerID, slotDependency.satisfiesValue(dependencyValues.at(layerID)));
	}
	return _climateDeterminer.expression.evaluate(values);
}

float Biome::closenessToClimate(const DependencyValues& values) const
{
	float result = 0.f;
	for (auto&& [id, val] : values)
	{
		if (!_climateDeterminer.dependencies.contains(id))
		{
			result += 1.0f;
			continue;
		}
		result += _climateDeterminer.dependencies.at(id).distanceFromValue(val);
	}
	return result;
}

std::vector<entt::id_type> Biome::getClimateDependencyIds() const
{
	return drft::util::extractKeys(_climateDeterminer.dependencies);
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

std::vector<entt::id_type> Biome::determineValidEntitySlots(const DependencyValues& dependencyValues) const
{
	std::vector<entt::id_type> result;
	for (auto&& [slotId, slotDeterminer] : _entitySlotDeterminers)
	{
		TokenValues values;
		for (auto&& [layerID, slotDependency] : slotDeterminer.dependencies)
		{
			if (!dependencyValues.contains(layerID)) continue;
			values.emplace(layerID, slotDependency.satisfiesValue(dependencyValues.at(layerID)));
		}
		if (slotDeterminer.expression.evaluate(values))
		{
			result.emplace_back(slotId);
		}
	}
	return result;
}

std::vector<entt::id_type> Biome::getFeatureDependencyIds() const
{
	std::vector<entt::id_type> result;
	for (auto&& [slotID, determiner] : _featureDeterminers)
	{
		auto ids = drft::util::extractKeys(determiner.dependencies);
		result.insert(result.end(), std::make_move_iterator(ids.begin()), std::make_move_iterator(ids.end()));
	}
	return result;
}

std::vector<entt::id_type> Biome::determineValidFeatures(const DependencyValues& dependencyValues) const
{
	std::vector<entt::id_type> result;
	for (auto&& [slotId, slotDeterminer] : _featureDeterminers)
	{
		TokenValues values;
		for (auto&& [layerID, slotDependency] : slotDeterminer.dependencies)
		{
			if (!dependencyValues.contains(layerID)) continue;
			values.emplace(layerID, slotDependency.satisfiesValue(dependencyValues.at(layerID)));
		}
		if (slotDeterminer.expression.evaluate(values))
		{
			result.push_back(slotId);
		}
	}
	return result;
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


