#pragma once

#include "Snapshot/Reflection.h"
#include <Utility/StandardLogger.h>

enum class SharpnessLevel : size_t
{
	Red = 0,
	Orange,
	Yellow,
	Green,
	Blue,
	White,
	Purple,

	_Count
};

static inline const std::unordered_map<SharpnessLevel, float> SharpnessLevelMultipliers =
{
	{ SharpnessLevel::Red,    0.5f },
	{ SharpnessLevel::Orange, 0.75f },
	{ SharpnessLevel::Yellow, 1.0f },
	{ SharpnessLevel::Green,  1.125f },
	{ SharpnessLevel::Blue,   1.25f },
	{ SharpnessLevel::White,  1.35f },
	{ SharpnessLevel::Purple, 1.50f },
};

struct SharpnessComponent
{
	std::array<int, std::to_underlying(SharpnessLevel::_Count)> levels;
	SharpnessLevel currentLevel = SharpnessLevel::Red;

	void modify(int amount)
	{
		_current = std::clamp(_current + amount, 0, _total);
		currentLevel = getCurrentSharpness();
	}

private:
	int _total = 0;
	int _current = 0;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "sharpness";

	static void setFromJSON(SharpnessComponent& comp, const rapidjson::Value& json)
	{
		if (json.HasMember("levels"))
		{
			std::fill(comp.levels.begin(), comp.levels.end(), 0);

			auto array = json["levels"].GetArray();
			size_t size = array.Size();
			if (size >= std::to_underlying(SharpnessLevel::_Count))
			{
				size = std::to_underlying(SharpnessLevel::_Count);
				LOG_WARNING("Sharpness component has too many values in 'levels'");
			}
			
			for (size_t i = 0; i < size; i++)
			{
				int val = array[i].GetInt();
				comp.levels[i] = val;
				comp._total += val;
			}

			comp.currentLevel = comp.getCurrentSharpness();
		}
		
	}

	SharpnessLevel getCurrentSharpness() const
	{
		int count = 0;
		for (int i = 0; i < levels.size(); i++)
		{
			count += levels[i];
			if (count > _current)
			{
				return static_cast<SharpnessLevel>(std::max(0, i - 1));
			}
		}
	}

	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SharpnessComponent, NAME>(ctx)
			.func<&SharpnessComponent::setFromJSON>("set_from_json"_hs);
	}
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(levels, _total, _current);
	}
};









