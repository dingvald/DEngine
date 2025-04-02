#pragma once
#include "Biome.h"
#include <JSON/ICreateFromJson.h>

class BiomeRegistry : public ICreateFromJson
{
public:
	BiomeRegistry();

	void createFromJson(const rapidjson::Value& json) override;
	const Biome& get(const std::string& name) const;
	void forEachBiome(std::function<void(const std::string&, const Biome&)> callback) const;

private:
	std::unordered_map<std::string, Biome> _biomes;
};

