#include "pch.h"
#include "EntityRenderer.h"
#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/LitComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/SpriteBatch.h"
#include "Spatial/Conversions.h"
#include "RenderLayers.h"
#include "LightingSystem.h"

static const sf::Color seenTileColor = sf::Color(12, 12, 12);

void drft::system::EntityRenderer::init()
{
	using namespace entt::literals;
	_sprites = _registry->ctx().get<sf::Texture&>("sprites"_hs);
	for (int l = 0; l < static_cast<int>(RenderLayer::Total); ++l)
	{
		_spriteLayers[l].setTexture(_sprites);
	}
}

void drft::system::EntityRenderer::render(sf::RenderTarget& target)
{
	auto camera = getCurrentCamera(*_registry);
	// Apply lighting to entities in the player's FOV
	const auto view = _registry->view< const PositionComponent, const RenderComponent, const LitComponent, const component::tag::InPlayerFOV, component::tag::InViewport>(entt::exclude<VisualEffectComponent>);
	for (auto const & [entity, pos, ren, lit] : view.each())
	{
		auto finalColor = LightingSystem::blendLight(ren.color, lit.color);
		finalColor.a = ren.color.a;

		sf::Vector2f renderPosition = toScreenSpace(pos.position, camera);
		_spriteLayers[ren.layer].addSprite(ren.sprite, finalColor, renderPosition);
	}
	// Apply darkened light to entities outside the player's FOV
	const auto seenView = _registry->view< const PositionComponent, const RenderComponent, const component::tag::PlayerHasSeen, component::tag::InViewport>(entt::exclude<component::tag::InPlayerFOV>);
	for (auto const& [entity, pos, ren] : seenView.each())
	{
		sf::Vector2f renderPosition = toScreenSpace(pos.position, camera);
		_spriteLayers[ren.layer].addSprite(ren.sprite, seenTileColor, renderPosition);
	}
	// Draw batches
	for (auto& [layer, batch] : _spriteLayers)
	{
		target.draw(batch);
		batch.clear();
	}
}
