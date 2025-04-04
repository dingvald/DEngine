#include "pch.h"
#include "BiomeRegistry.h"
#include "JSON/JSONHelpers.h"
#include <Utility/StandardLogger.h>

using namespace drft;

BiomeRegistry::BiomeRegistry()
{
}

void BiomeRegistry::loadBiomes(const std::filesystem::path& biomesDirectory)
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator(biomesDirectory))
	{
		if (entry.is_directory()) continue;

		json::JsonFileWrapper json{ entry.path(), "biomes" };
		if (!json.load())
		{
			//LOG_WARNING("{} could not be loaded", entry);
			continue;
		}
		for (auto&& biomeObj : json.getRoot().GetObject())
		{
			std::string name = biomeObj.name.GetString();
			Biome biome{ name };
			biome.createFromJSON(biomeObj.value);
			_biomes.emplace(std::move(name), std::move(biome));
		}
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


