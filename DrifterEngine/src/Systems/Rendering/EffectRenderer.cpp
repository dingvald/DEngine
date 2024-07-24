#include "pch.h"
#include "EffectRenderer.h"

#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/Tags.h"

#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/SpriteBatch.h"
#include "Utility/TextureAtlas.h"
#include "Spatial/Conversions.h"
#include "RenderLayers.h"

void drft::system::EffectRenderer::init()
{
	using namespace entt::literals;
	_textureAtlas = &_registry.ctx().get<const TextureAtlas&>();
	for (int l = 0; l < static_cast<int>(RenderLayer::Total); ++l)
	{
		_spriteLayers[l].setTexture(_textureAtlas->getTexture());
	}
}

void drft::system::EffectRenderer::render(sf::RenderTarget& target)
{
	auto camera = getCurrentCamera(_registry);
	const auto view = _registry.view< const PositionComponent, const RenderComponent, const VisualEffectComponent, component::tag::InViewport>();
	for (auto const& [entity, pos, ren, effect] : view.each())
	{
		if (effect.requiresInFOV && !_registry.all_of<component::tag::InPlayerFOV>(entity))
		{
			continue;
		}

		sf::Vector2f renderPosition = toScreenSpace(pos.position, camera);
		sf::IntRect uv = _textureAtlas->getUV(ren.texture, ren.uvSize, ren.uvCoords);
		_spriteLayers[ren.layer].addSprite(uv, ren.color, renderPosition);
	}
	// Draw batches
	for (auto& [layer, batch] : _spriteLayers)
	{
		target.draw(batch);
		batch.clear();
	}
}
