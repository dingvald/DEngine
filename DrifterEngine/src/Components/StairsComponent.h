#pragma once

#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Utility/StandardLogger.h>


struct StairsComponent
{
	enum class Type
	{
		Up,
		Down
	};
	Type type = Type::Down;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "stairs";

	static void setFromJSON(StairsComponent& stairs, const rapidjson::Value& json)
	{
		if (json.HasMember("type"))
		{
			std::string type = json["type"].GetString();
			if (type == "down") stairs.type = Type::Down;
			else if (type == "up") stairs.type = Type::Up;
			else
			{
				LOG_ERROR("Unknown stairs type {}", type);
				stairs.type = Type::Down;
			}
		}
		else
		{
			LOG_ERROR("'stairs' component requires a type, but none provided");
			stairs.type = Type::Down;
		}
	}

	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<StairsComponent, NAME>(ctx)	
			.func<&StairsComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, StairsComponent& stairs)
	{
		archive(stairs.type);
	}
}

