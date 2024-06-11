#pragma once
#include <Utility/Vector2Serialization.h>
#include <Utility/ColorSerialization.h>

struct RenderComponent;

struct SpriteOptions
{
	std::optional<sf::Vector2i> uvCoords;
	std::optional<entt::id_type> texture;
	std::optional<sf::Vector2i> uvSize;
	std::optional<int> layer;
	std::optional<sf::Color> color;

private:
	friend class cereal::access;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(uvCoords, texture, uvSize, layer, color);
	}
};

void createSpriteOptionsFromRenderComponent(SpriteOptions& options, const RenderComponent& render);

void applySpriteOptionsToRenderComponent(RenderComponent& render, const SpriteOptions& options);