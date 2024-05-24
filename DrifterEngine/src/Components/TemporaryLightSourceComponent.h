#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct TemporaryLightSourceComponent
{
	float radius = 1.f;
	float dropOff = 0.1f;
	sf::Color color = sf::Color::White;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "TemporaryLightSource";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<TemporaryLightSourceComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, TemporaryLightSourceComponent& temporaryLightSource)
	{
		archive(
			temporaryLightSource.radius,
			temporaryLightSource.dropOff,
			temporaryLightSource.color.r,
			temporaryLightSource.color.g,
			temporaryLightSource.color.b
		);
	}
}




