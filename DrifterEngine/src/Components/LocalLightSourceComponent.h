#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct LocalLightSourceComponent
{
	float radius = 1.f;
	sf::Color color = sf::Color::White;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "local_light_source";
	static void bind(entt::meta_ctx& ctx)
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LocalLightSourceComponent, NAME>(ctx)
			.data<&LocalLightSourceComponent::radius>("radius"_hs)
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
			localLightSource.color.r, 
			localLightSource.color.g, 
			localLightSource.color.b
		);
	}
}



