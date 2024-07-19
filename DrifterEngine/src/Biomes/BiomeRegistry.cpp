#include "pch.h"
#include "BiomeRegistry.h"
#include "JSON/JSONHelpers.h"

#include <ProcGen/ProbabilityMultipliers/NeighborMultiplier.h>
#include <ProcGen/ProbabilityMultipliers/NoiseLayerMultiplier.h>

BiomeRegistry::BiomeRegistry()
{
	MuliplierFactory::registerType<drft::gen::NeighborMultiplier>("neighbor");
	MuliplierFactory::registerType<drft::gen::NoiseLayerMultiplier>("noise_layer");
}

void BiomeRegistry::createBiomesFromJSON(const std::filesystem::path& directoryPath)
{
	for (const auto& filename : std::filesystem::directory_iterator(directoryPath))
	{
		auto biomeObj = drft::json::extractDOM(filename.path(), "Biomes");
		if (!biomeObj.has_value())
		{
			std::cout << "Failure: " << directoryPath << " could not be parsed." << std::endl;
		}
		else
		{
			for (auto&& val : biomeObj.value()["Biomes"].GetObject())
			{
				std::string name = val.name.GetString();
				Biome biome{name};
				biome.createFromJSON(val.value);
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
