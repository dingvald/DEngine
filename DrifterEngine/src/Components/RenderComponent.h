#pragma once

#include "SFML/Graphics/Color.hpp"
#include <string>
#include "EnTT/entt.hpp"
#include "Snapshot/Reflection.h"
#include "Utility/Vector2Serialization.h"

static inline const sf::Vector2i DefaultTileTextureSize = { 16, 24 };

struct RenderComponent
{
	entt::id_type texture;
	sf::Vector2i uvSize = DefaultTileTextureSize;
	sf::Vector2i uvCoords;
	unsigned int layer = 0u;
	sf::Color color = sf::Color::Magenta;

private:
	friend class ComponentMetaBinder;
	static inline const std::string_view NAME = "Render";
	static int textureSetter(RenderComponent& r, std::string val)
	{
		return r.texture = entt::hashed_string(val.c_str()).value();
	}
	static void bind()
	{
		using namespace entt::literals;
		snapshot::reflectComponent<RenderComponent, NAME>()
			.prop("serialize"_hs)
			.data<&RenderComponent::textureSetter, &RenderComponent::texture>("texture"_hs)
			.data<&RenderComponent::uvSize>("uv_size"_hs)
			.data<&RenderComponent::uvCoords>("uv_coords"_hs)
			.data<&RenderComponent::layer>("layer"_hs)
			.data<&RenderComponent::color>("color"_hs);
	}
};

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, RenderComponent& render)
	{
		archive(
			render.texture,
			render.uvSize,
			render.uvCoords,
			render.layer,
			render.color.r,
			render.color.g,
			render.color.b);
	}
}

static const RenderComponent DebugRenderComponent =
{
	.texture = entt::hashed_string("rectangle"),
	.uvSize = DefaultTileTextureSize,
	.uvCoords = {0, 0},
	.layer = 2u,
	.color = sf::Color::Magenta
};