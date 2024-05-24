#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"

struct RenderComponent
{
	unsigned int sprite = 4u;
	unsigned int layer = 0u;
	sf::Color color = sf::Color::Magenta;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Render";
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<RenderComponent, NAME>()
			.prop("serialize"_hs)
			.data<&RenderComponent::sprite>("sprite"_hs)
			.data<&RenderComponent::layer>("layer"_hs)
			.data<&RenderComponent::color>("color"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, RenderComponent& render)
	{
		archive(render.sprite, render.layer, render.color.r, render.color.g, render.color.b);
	}
}

