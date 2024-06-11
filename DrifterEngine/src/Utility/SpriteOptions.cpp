#include "pch.h"
#include "SpriteOptions.h"
#include "Components/RenderComponent.h"

void createSpriteOptionsFromRenderComponent(SpriteOptions& options, const RenderComponent& render)
{
	options.uvCoords = render.uvCoords;
	options.texture = render.texture;
	options.uvSize = render.uvSize;
	options.layer = render.layer;
	options.color = render.color;
}

void applySpriteOptionsToRenderComponent(RenderComponent& render, const SpriteOptions& options)
{
	render.uvCoords = options.uvCoords.value_or(render.uvCoords);
	render.texture = options.texture.value_or(render.texture);
	render.uvSize = options.uvSize.value_or(render.uvSize);
	render.layer = options.layer.value_or(render.layer);
	render.color = options.color.value_or(render.color);
}