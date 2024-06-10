#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include "EnTT/entt.hpp"
#include "Cereal/external/rapidjson/document.h"
#include "Snapshot/Reflection.h"
#include "AnimationComponent.h"
#include "Utility/SpriteOptions.h"



struct SpriteControllerComponent
{
private:
	struct StateNode
	{
		SpriteOptions options;
		std::optional<float> speed; // Frames / second

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(options, speed);
		}
	};
public:
	std::unordered_map<entt::id_type, std::vector<StateNode>> states;

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
					node.options.uvCoords = sf::Vector2i{};
					node.options.uvCoords.value().x = obj["uv_coords"].GetArray()[0].GetInt();
					node.options.uvCoords.value().y = obj["uv_coords"].GetArray()[1].GetInt();
				}
				if (obj.HasMember("texture"))
				{
					node.options.texture = entt::hashed_string(obj["texture"].GetString());
				}
				if (obj.HasMember("speed"))
				{
					node.speed = obj["speed"].GetFloat();
				}
				if (obj.HasMember("uv_size"))
				{
					node.options.uvSize = { 0,0 };
					node.options.uvSize.value().x = obj["uv_size"].GetArray()[0].GetInt();
					node.options.uvSize.value().y = obj["uv_size"].GetArray()[1].GetInt();
				}
				if (obj.HasMember("layer"))
				{
					node.options.layer = obj["layer"].GetInt();
				}
				if (obj.HasMember("color"))
				{
					node.options.color = sf::Color{};
					node.options.color.value().r = obj["color"].GetArray()[0].GetInt();
					node.options.color.value().g = obj["color"].GetArray()[1].GetInt();
					node.options.color.value().b = obj["color"].GetArray()[2].GetInt();
				}

				nodes.emplace_back(std::move(node));
			}
			spriteController.states.emplace(entt::hashed_string(name.GetString()), std::move(nodes));
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