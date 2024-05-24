#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct ProjectileComponent
{
	std::vector<sf::Vector2i> line;
	int progress = 0;
	float speed = 5.0f;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Projectile";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<ProjectileComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, ProjectileComponent& projectile)
	{
		archive(
			projectile.line,
			projectile.progress,
			projectile.speed
		);

	}
}




