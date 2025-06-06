#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct TemporaryLightSourceComponent
{
	float radius = 1.f;
	sf::Color color = sf::Color::White;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "temporary_light_source";
	static void bind(entt::meta_ctx& ctx)
	{
		snapshot::reflectComponent<TemporaryLightSourceComponent, NAME>(ctx);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, TemporaryLightSourceComponent& temporaryLightSource)
	{
		archive(
			temporaryLightSource.radius,
			temporaryLightSource.color.r,
			temporaryLightSource.color.g,
			temporaryLightSource.color.b
		);
	}
}




