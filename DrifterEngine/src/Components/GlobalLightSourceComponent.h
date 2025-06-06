#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct GlobalLightSourceComponent
{
	sf::Color color = sf::Color::White;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "global_light_source";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<GlobalLightSourceComponent, NAME>(ctx)
			.data<&GlobalLightSourceComponent::color>("color"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, GlobalLightSourceComponent& globalLightSource)
	{
		archive(
			globalLightSource.color.r,
			globalLightSource.color.g,
			globalLightSource.color.b
		);
	}
}




