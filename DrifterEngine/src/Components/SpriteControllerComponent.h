#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include "EnTT/entt.hpp"
#include "Cereal/external/rapidjson/document.h"
#include "Snapshot/Reflection.h"
#include "AnimationComponent.h"
#include "Utility/Vector2Serialization.h"



struct SpriteControllerComponent
{
private:
	struct StateNode
	{
		sf::Vector2i uvCoords;
		std::optional<std::string> next;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(uvCoords, next);
		}
	};
public:
	std::unordered_map<std::string, std::vector<StateNode>> states;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "SpriteController";

	static void setFromJSON(SpriteControllerComponent& spriteController, const rapidjson::Value& json)
	{
		if (!json.HasMember("states")) return;

		for (auto&& [name, value] : json["states"].GetObject())
		{
			std::vector<StateNode> nodes;
			for (auto&& val : value.GetArray())
			{
				StateNode node;
				auto obj = val.GetObject();
				if (obj.HasMember("uv_coords"))
				{
					node.uvCoords.x = obj["uv_coords"].GetArray()[0].GetInt();
					node.uvCoords.y = obj["uv_coords"].GetArray()[1].GetInt();
				}
				if (obj.HasMember("next"))
				{
					node.next = obj["next"].GetString();
				}
				nodes.emplace_back(std::move(node));
			}
			spriteController.states.emplace(name.GetString(), std::move(nodes));
		}
	}

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<SpriteControllerComponent, NAME>()
			.prop("serialize"_hs)
			.func<&SpriteControllerComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, SpriteControllerComponent& spriteController)
	{
		archive(spriteController.states);
	}
}