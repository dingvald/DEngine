#include "pch.h"
#include "TileRenderer.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"

void drft::system::TileRenderer::init()
{
	using namespace entt::literals;
	_spriteBatch.setTexture(registry->ctx().get<sf::Texture&>("sprites"_hs));

	// Build the tile sprite batch
	for (int y = 0; y < spatial::CHUNK_HEIGHT; ++y)
	{
		for (int x = 0; x < spatial::CHUNK_WIDTH; ++x)
		{
			_spriteBatch.addSprite(3, sf::Color(90, 90, 90, 255), spatial::toWorldSpace({ x,y }));
		}
	}
}

void drft::system::TileRenderer::render(sf::RenderTarget& target)
{
	sf::Vector2f cameraOrigin = { 0,0 };
	sf::Vector2f cameraCenter = { 0,0 };
	auto cameraView = registry->view<const component::Camera, const component::Position>();
	for (auto const& [entity, cam, pos] : cameraView.each())
	{
		cameraOrigin = { cam.viewport.left, cam.viewport.top };
		cameraCenter = pos.position;
	}

	auto tileDiff = spatial::toTileSpace(cameraCenter) - spatial::toTileSpace(_prevPosition);
	_spriteBatch.setOrigin(cameraCenter);
	_spriteBatch.move(spatial::toWorldSpace(tileDiff));
	_prevPosition = cameraCenter;

	target.draw(_spriteBatch);
}
