#include "pch.h"
#include "EntityRenderer.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/SpriteBatch.h"
#include "Spatial/Conversions.h"
#include "RenderLayers.h"

static const sf::Color seenTileColor = sf::Color(12, 12, 12);

void drft::system::EntityRenderer::init()
{
	using namespace entt::literals;
	_sprites = registry->ctx().get<sf::Texture&>("sprites"_hs);
	for (int l = 0; l < static_cast<int>(RenderLayer::Total); ++l)
	{
		_spriteLayers[l].setTexture(_sprites);
	}
}

void drft::system::EntityRenderer::render(sf::RenderTarget& target)
{
	auto camera = getCurrentCamera(*registry);
	// Apply lighting to entities in the player's FOV
	const auto view = registry->view< const component::Position, const component::Render, const component::Lit, const component::tag::InPlayerFOV, component::tag::InViewport>(entt::exclude<component::Effect>);
	for (auto const & [entity, pos, ren, lit] : view.each())
	{
		sf::Uint8 r = static_cast<sf::Uint8>(std::clamp(ren.color.r * (static_cast<float>(lit.color.r) / 255.f), 0.f, 255.f));
		sf::Uint8 g = static_cast<sf::Uint8>(std::clamp(ren.color.g * (static_cast<float>(lit.color.g) / 255.f), 0.f, 255.f));
		sf::Uint8 b = static_cast<sf::Uint8>(std::clamp(ren.color.b * (static_cast<float>(lit.color.b) / 255.f), 0.f, 255.f));
		sf::Vector2f renderPosition = toScreenSpace(pos.position, camera);
		_spriteLayers[ren.layer].addSprite(ren.sprite, sf::Color(r,g,b, ren.color.a), renderPosition);
	}
	// Apply darkened light to entities outside the player's FOV
	const auto seenView = registry->view< const component::Position, const component::Render, const component::PlayerHasSeen, component::tag::InViewport>(entt::exclude<component::tag::InPlayerFOV>);
	for (auto const& [entity, pos, ren, seen] : seenView.each())
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
