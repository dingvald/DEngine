#pragma once
#include "EnTT/entt.hpp"
#include <AI/Blackboard/Blackboard.h>
#include "Snapshot/Reflection.h"

struct UtilityAIComponent
{
	entt::id_type archetype;
	Blackboard blackboard;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "ai_input";

	static void setFromJson(UtilityAIComponent& comp, const rapidjson::Value& json)
	{
		if (json.HasMember("archetype"))
		{
			comp.archetype = entt::hashed_string{ json["archetype"].GetString() };
		}
	}

	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<UtilityAIComponent, NAME>(ctx)
			.func<&UtilityAIComponent::setFromJson>("set_from_json"_hs);
	}

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(archetype);
	}
};