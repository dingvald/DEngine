#include "pch.h"
#include "BiomeRegistry.h"
#include "JSON/JSONHelpers.h"


BiomeRegistry::BiomeRegistry()
{
}

void BiomeRegistry::createBiomesFromJSON(const std::filesystem::path& directoryPath)
{
	for (const auto& filename : std::filesystem::directory_iterator(directoryPath))
	{
		drft::json::JsonRootExtractor jsonRootExtractor{ filename.path(), "Biomes" };
		if (!jsonRootExtractor.isValid())
		{
			std::cout << "Failure: " << directoryPath << " could not be parsed." << std::endl;
		}
		else
		{
			for (auto&& biomeObj : jsonRootExtractor.getRoot().GetObject())
			{
				std::string name = biomeObj.name.GetString();
				Biome biome{name};
				biome.createFromJSON(biomeObj.value);
				_biomes.emplace(std::move(name), std::move(biome));
			}
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
