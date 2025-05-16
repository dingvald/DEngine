#pragma once

#include "Snapshot/Reflection.h"
#include <vector>

struct AbilitiesComponent
{
	std::vector<entt::id_type> abilities;
private:

	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "abilities";

	static void setFromJSON(AbilitiesComponent& comp, const rapidjson::Value& json)
	{
		for (auto&& val : json.GetArray())
		{
			entt::id_type abilityId = entt::hashed_string{ val.GetString() };
			comp.abilities.push_back(abilityId);
		}
	}

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<AbilitiesComponent, NAME>()
			.prop("serialize"_hs)
			.func<&AbilitiesComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, AbilitiesComponent& abilities)
	{
		archive(abilities.abilities);
	}
}