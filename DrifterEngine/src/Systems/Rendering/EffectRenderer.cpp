#include "pch.h"
#include "EffectRenderer.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/SpriteBatch.h"
#include "Spatial/Conversions.h"
#include "RenderLayers.h"

void drft::system::EffectRenderer::init()
{
	using namespace entt::literals;
	_sprites = _registry->ctx().get<sf::Texture&>("sprites"_hs);
	for (int l = 0; l < static_cast<int>(RenderLayer::Total); ++l)
	{
		_spriteLayers[l].setTexture(_sprites);
	}
}

void drft::system::EffectRenderer::render(sf::RenderTarget& target)
{
	auto camera = getCurrentCamera(*_registry);
	const auto view = _registry->view< const component::Position, const component::Render, const component::Effect, component::tag::InViewport>();
	for (auto const& [entity, pos, ren, effect] : view.each())
	{
		if (effect.requiresInFOV && !_registry->all_of<component::tag::InPlayerFOV>(entity))
		{
			continue;
		}
		sf::Vector2f renderPosition = toScreenSpace(pos.position, camera);
		_spriteLayers[ren.layer].addSprite(ren.sprite, ren.color, renderPosition);
	}
	// Draw batches
	for (auto& [layer, batch] : _spriteLayers)
	{
		target.draw(batch);
		batch.clear();
	}
}
