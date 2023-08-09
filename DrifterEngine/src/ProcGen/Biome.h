#pragma once

namespace drft::gen
{
	enum class TemperatureRange
	{
		Cold,
		Cool,
		Warm,
		Hot
	};
	const std::unordered_map<std::string, TemperatureRange> TemperatureString2Enum =
	{
		{"Cold", TemperatureRange::Cold},
		{"Cool", TemperatureRange::Cool},
		{"Warm", TemperatureRange::Warm},
		{"Hot", TemperatureRange::Hot},
	};

	enum class HumidityRange
	{
		Dry,
		Moderate,
		Humid,
		VeryHumid
	};
	const std::unordered_map<std::string, HumidityRange> HumidityString2Enum =
	{
		{"Dry", HumidityRange::Dry},
		{"Moderate", HumidityRange::Moderate},
		{"Humid", HumidityRange::Humid},
		{"Very Humid", HumidityRange::VeryHumid}
	};

	enum class AltitudeRange
	{
		Low,
		Medium,
		High,
		VeryHigh
	};
	const std::unordered_map<std::string, AltitudeRange> AltitudeString2Enum =
	{
		{"Low", AltitudeRange::Low},
		{"Medium", AltitudeRange::Medium},
		{"High", AltitudeRange::High},
		{"Very High", AltitudeRange::VeryHigh}
	};

	struct BiomeIcon
	{
		unsigned int sprite = 4;
		sf::Color color = sf::Color::Magenta;
	};

	struct Biome
	{
		std::string name;
		BiomeIcon icon;
		std::unordered_set<TemperatureRange> temperatures;
		std::unordered_set<HumidityRange> humidities;
		std::unordered_set<AltitudeRange> altitudes;
	};
}



