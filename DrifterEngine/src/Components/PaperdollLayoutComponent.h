#pragma once

#include "Systems/HelperClasses/BodyPart.h"
#include <string>
#include <array>
#include <optional>
#include <SFML/System.hpp>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include <Utility/StandardLogger.h>
#include <utility/stdHashing.h>


struct PaperdollLayoutComponent
{
	sf::Vector2i dimensions;
	std::unordered_map<sf::Vector2i, std::string> layout;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "paperdoll_layout";

	static void setFromJSON(PaperdollLayoutComponent& body, const rapidjson::Value& json)
	{
		std::unordered_map<int, std::string> keys;
		if (json.HasMember("keys"))
		{
			for (auto&& member : json["keys"].GetObject())
			{
				std::string name = member.name.GetString();
				int value = member.value.GetInt();
				keys.emplace(value, std::move(name));
			}
		}
		if (json.HasMember("layout"))
		{
			int x = 0;
			int y = 0;

			for (auto&& row : json["layout"].GetArray())
			{
				for (auto&& val : row.GetArray())
				{
					int key = val.GetInt();
					if (key != 0)
					{
						if (!keys.contains(key))
						{
							error_logger << "Error: could not parse paperdoll component - layout contains unknown key " << key << std::endl;
							return;
						}
						sf::Vector2i position = { x, y };
						if (body.layout.contains(position))
						{
							error_logger << "Error: could not parse paperdoll component - key " << key << " appears more than once" << std::endl;
							return;
						}
						body.layout.emplace(position, keys.at(key));
					}
					x++;
					body.dimensions.x = std::max(x, body.dimensions.x);
				}
				x = 0;
				y++;
				body.dimensions.y = std::max(y, body.dimensions.y);
			}
		}
	}

	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<PaperdollLayoutComponent, NAME>()
			.prop("serialize"_hs)
			.func<&PaperdollLayoutComponent::setFromJSON>("set_from_json"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, PaperdollLayoutComponent& body)
	{
		archive(body.dimensions, body.layout);
	}
}