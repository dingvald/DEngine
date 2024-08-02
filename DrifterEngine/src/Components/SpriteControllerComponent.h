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
public:
	struct StateNode
	{
		std::vector<SpriteOptions> frames;
		std::optional<float> speed; // Seconds / Frame
		bool synced = false;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(frames, speed, synced);
		}
	};
public:
	std::unordered_map<entt::id_type, std::variant<entt::id_type, StateNode>> states;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "SpriteController";

	static void setFromJSON(SpriteControllerComponent& spriteController, const rapidjson::Value& json)
	{
		if (!json.HasMember("states")) return;

		for (auto&& [name, value] : json["states"].GetObject())
		{
			if (value.IsString())
			{
				spriteController.states.emplace(entt::hashed_string(name.GetString()), entt::hashed_string(value.GetString()));
			}
			else if (value.IsObject())
			{
				StateNode node;
				if (value.HasMember("frames"))
				{
					for (auto&& val : value["frames"].GetArray())
					{
						SpriteOptions frame;
						auto obj = val.GetObject();
						if (obj.HasMember("uv_coords"))
						{
							frame.uvCoords = sf::Vector2i{};
							frame.uvCoords.value().x = obj["uv_coords"].GetArray()[0].GetInt();
							frame.uvCoords.value().y = obj["uv_coords"].GetArray()[1].GetInt();
						}
						if (obj.HasMember("texture"))
						{
							frame.texture = entt::hashed_string(obj["texture"].GetString());
						}
						if (obj.HasMember("uv_size"))
						{
							frame.uvSize = { 0,0 };
							frame.uvSize.value().x = obj["uv_size"].GetArray()[0].GetInt();
							frame.uvSize.value().y = obj["uv_size"].GetArray()[1].GetInt();
						}
						if (obj.HasMember("layer"))
						{
							frame.layer = obj["layer"].GetInt();
						}
						if (obj.HasMember("color"))
						{
							frame.color = sf::Color{};
							frame.color.value().r = obj["color"].GetArray()[0].GetInt();
							frame.color.value().g = obj["color"].GetArray()[1].GetInt();
							frame.color.value().b = obj["color"].GetArray()[2].GetInt();
						}

						node.frames.emplace_back(std::move(frame));
					}
				}
				if (value.HasMember("speed"))
				{
					node.speed = value["speed"].GetFloat();
				}
				if (value.HasMember("synced"))
				{
					node.synced = value["synced"].GetBool();
				}
				spriteController.states.emplace(entt::hashed_string(name.GetString()), std::move(node));
			}
			
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