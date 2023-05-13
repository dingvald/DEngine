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
	sf::FloatRect viewport;
	sf::FloatRect bufferdViewport;
	const auto camera = registry->view<const component::Camera, const component::Position>();

	// assumes only one camera, otherwise will just use the last one's position
	for (auto const& [entity, cam, pos] : camera.each())
	{
		viewport = cam.viewport;
		bufferdViewport = {
			viewport.left - 32,
			viewport.top - 32,
			viewport.width + 64,
			viewport.height + 64
		};
	}

	const auto view = registry->view< const component::Position, const component::Render, const component::Lit>();
	for (auto const & [entity, pos, ren, lit] : view.each())
	{
		auto tempLit = lit;
		registry->remove<component::Lit>(entity);
		if (!bufferdViewport.contains(pos.position)) continue;
		sf::Uint8 r = static_cast<sf::Uint8>(std::clamp(ren.color.r * static_cast<float>(tempLit.color.r / 255), 0.f, 255.f));
		sf::Uint8 g = static_cast<sf::Uint8>(std::clamp(ren.color.g * static_cast<float>(tempLit.color.g / 255), 0.f, 255.f));
		sf::Uint8 b = static_cast<sf::Uint8>(std::clamp(ren.color.b * static_cast<float>(tempLit.color.b / 255), 0.f, 255.f));
		float x = std::round(pos.position.x - viewport.left);
		float y = std::round(pos.position.y - viewport.top);
		_spriteLayers[pos.depth].addSprite(ren.sprite, sf::Color(r,g,b,ren.color.a), {x, y});
	}

	for (auto& [depth, batch] : _spriteLayers)
	{
		target.draw(batch);
		batch.clear();
	}
}
