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
	static inline const std::string_view NAME = "GlobalLightSource";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<GlobalLightSourceComponent, NAME>()
			.prop("serialize"_hs)
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




