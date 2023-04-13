#include "pch.h"
#include "EntityRenderer.h"
#include "Components/Components.h"
#include "Utility/SpriteBatch.h"
#include "Spatial/Conversions.h"


void drft::system::EntityRenderer::init()
{
	using namespace entt::literals;
	_sprites = registry->ctx().get<sf::Texture&>("sprites"_hs);
	for (int l = 0; l < (int)spatial::Layer::Total; ++l)
	{
		_spriteLayers[l].setTexture(_sprites);
	}
}

void drft::system::EntityRenderer::render(sf::RenderTarget& target)
{
	sf::Vector2f cameraOrigin = { 0.0f, 0.0f };
	const auto camera = registry->view<const component::Camera, const component::Position>(entt::exclude<component::Prototype>);

	// assumes only one camera, otherwise will just use the last one's position
	for (auto const& [entity, cam, pos] : camera.each())
	{
		cameraOrigin = { cam.viewport.left, cam.viewport.top };
	}

	const auto view = registry->view< const component::Position, const component::Render >(entt::exclude<component::Prototype>);

	for (auto const & [entity, pos, ren] : view.each())
	{
		float x = std::round(pos.position.x - cameraOrigin.x);
		float y = std::round(pos.position.y - cameraOrigin.y);
		_spriteLayers[pos.depth].addSprite(ren.sprite, ren.color, {x, y});
	}

	for (auto& [depth, batch] : _spriteLayers)
	{
		target.draw(batch);
		batch.clear();
	}
}
