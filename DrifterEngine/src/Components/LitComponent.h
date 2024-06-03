#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"


struct LitComponent
{
	sf::Color color = sf::Color::Black;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Lit";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<LitComponent, NAME>()
			.prop("serialize"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, LitComponent& lit)
	{
		archive(lit.color.r, lit.color.g, lit.color.b);
	}
}


