#include "pch.h"
#include "BiomeRegistry.h"
#include "JSON/JSONHelpers.h"


BiomeRegistry::BiomeRegistry()
{
}

void BiomeRegistry::createFromJson(const rapidjson::Value& json)
{
	for (auto&& biomeObj : json.GetObject())
	{
		std::string name = biomeObj.name.GetString();
		Biome biome{ name };
		biome.createFromJSON(biomeObj.value);
		_biomes.emplace(std::move(name), std::move(biome));
	}
}

const Biome& BiomeRegistry::get(const std::string& name) const
{
	return _biomes.at(name);
}

void BiomeRegistry::forEachBiome(std::function<void(const std::string&, const Biome&)> callback) const
{
	for (auto&& [name, biome] : _biomes)
	{
		callback(name, biome);
	}
}


