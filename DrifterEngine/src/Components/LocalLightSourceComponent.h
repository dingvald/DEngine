#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct LocalLightSourceComponent
{
	float radius = 1.f;
	float dropOff = 0.1f;
	sf::Color color = sf::Color::White;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "local_light_source";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LocalLightSourceComponent, NAME>()
			.prop("serialize"_hs)
			.data<&LocalLightSourceComponent::radius>("radius"_hs)
			.data<&LocalLightSourceComponent::dropOff>("drop_off"_hs)
			.data<&LocalLightSourceComponent::color>("color"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, LocalLightSourceComponent& localLightSource)
	{
		archive(
			localLightSource.radius,
			localLightSource.dropOff, 
			localLightSource.color.r, 
			localLightSource.color.g, 
			localLightSource.color.b
		);
	}
}



