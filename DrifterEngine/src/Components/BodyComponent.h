#pragma once

#include "Systems/HelperClasses/BodyPart.h"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct BodyComponent
{
	BodyPartTree parts;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "body";

	static void setFromJSON(BodyComponent& body, const rapidjson::Value& json)
	{
		body.parts.createFromJson(json);
	}

	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<BodyComponent, NAME>(ctx)
			.func<&BodyComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, BodyComponent& body)
	{
		archive(body.parts);
	}
}











