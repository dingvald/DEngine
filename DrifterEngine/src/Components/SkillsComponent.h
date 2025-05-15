#pragma once
#include <string>
#include "Snapshot/Reflection.h"
#include <Skills/Skill.h>

struct SkillsComponent
{
	std::vector<Skill> skills;
private:
	static void setFromJSON(SkillsComponent& skills, const rapidjson::Value& json)
	{
		for (auto&& skillObj : json.GetObject())
		{
			const char* name = skillObj.name.GetString();
			const int val = skillObj.value.GetInt();
			skills.skills.emplace_back(name, val);
		}
	}

	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "skills";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SkillsComponent, NAME>()
			.prop("serialize"_hs)
			.func<&SkillsComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SkillsComponent& coreSkills)
	{
		archive(coreSkills.skills);
	}
}