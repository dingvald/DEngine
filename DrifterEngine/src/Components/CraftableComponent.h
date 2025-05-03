#pragma once

#include <string>
#include "Snapshot/Reflection.h"
#include <Systems/HelperClasses/CraftingIngredient.h>
#include <Skills/SkillRequirement.h>

struct CraftableComponent
{
	std::vector<CraftingIngredient> recipe;
	entt::id_type workstationID = entt::hashed_string{ "any" };
	std::vector<SkillRequirement> skillRequirements;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "craftable";
	static void setFromJSON(CraftableComponent& craftable, const rapidjson::Value& json)
	{
		if (json.HasMember("recipe"))
		{
			for (auto&& val : json["recipe"].GetArray())
			{
				CraftingIngredient newIngredient;
				newIngredient.createFromJson(val);
				craftable.recipe.emplace_back(std::move(newIngredient));
			}
		}
		if (json.HasMember("workstation"))
		{
			craftable.workstationID = entt::hashed_string{ json["workstation"].GetString() };
		}
		if (json.HasMember("skills_required"))
		{
			for (auto&& val : json["skills_required"].GetArray())
			{
				entt::id_type skillId = entt::hashed_string{ val.GetArray()[0].GetString() };
				int minLevel = val.GetArray()[1].GetInt();
				craftable.skillRequirements.emplace_back(skillId, minLevel);
			}
		}
	}
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<CraftableComponent, NAME>()
			.prop("serialize"_hs)
			.func<&CraftableComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, CraftableComponent& craftable)
	{
		archive(craftable.recipe);
	}
}