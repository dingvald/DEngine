#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct LevelingComponent
{
	unsigned int currentLevel = 0;
	unsigned int currentXP = 0;
	unsigned int neededXP = 100;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Leveling";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LevelingComponent, NAME>()
			.prop("serialize"_hs)
			.data<&LevelingComponent::currentLevel>("currentLevel"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, LevelingComponent& leveling)
	{
		archive(leveling.currentLevel, leveling.currentXP, leveling.neededXP);
	}
}


