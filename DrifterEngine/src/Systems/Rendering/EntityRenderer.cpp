#include "pch.h"
#include "EntityRenderer.h"
#include <Components/PlayerHasSeenComponent.h>
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"
#include "Components/LitComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/Tags.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Utility/SpriteBatch.h"
#include "Utility/TextureAtlas.h"
#include <Spatial/Helpers.h>
#include "RenderLayers.h"
#include "LightingSystem.h"

static const sf::Color SeenTileColor = sf::Color(10, 10, 10);

void drft::system::EntityRenderer::init()
{
	_textureAtlas = &_registry.ctx().get<const TextureAtlas&>();
	for (int l = 0; l < static_cast<int>(RenderLayer::Total); ++l)
	{
		_spriteLayers[l].setTexture(_textureAtlas->getTexture());
	}
}

void drft::system::EntityRenderer::render(sf::RenderTarget& target)
{
	const CameraHandle camera = getCurrentCamera(_registry);
	target.setView(camera.camera.view);

	batchLitEntities(camera);
	batchHadSeenEntities(camera);
	batchEffectEntities(camera);

	for (auto& [layer, batch] : _spriteLayers)
	{
		target.draw(batch);
		batch.clear();
	}

	target.setView(target.getDefaultView());
}

void drft::system::EntityRenderer::batchLitEntities(const CameraHandle& camera)
{
	const auto view = _registry.view< const PositionComponent, const RenderComponent, const LitComponent, component::tag::InPlayerFOV, component::tag::InViewport>(entt::exclude<VisualEffectComponent>);
	for (auto const& [entity, pos, ren, lit] : view.each())
	{
		auto finalColor = LightingSystem::blendLight(ren.color, lit.color);
		finalColor.a = ren.color.a;

		sf::Vector2f renderPosition = toScreenSpace(pos.tile, camera) + spatial::toXY(pos.offset);
		sf::IntRect uv = _textureAtlas->getUV(ren.texture, ren.uvSize, ren.uvCoords);
		_spriteLayers.at(ren.layer).addSprite(uv, finalColor, renderPosition);
	}
}

void drft::system::EntityRenderer::batchHadSeenEntities(const CameraHandle& camera)
{
	const auto seenView = _registry.view< const PositionComponent, const RenderComponent, const PlayerHasSeenComponent, component::tag::InViewport>(entt::exclude<component::tag::InPlayerFOV>);
	for (auto const& [entity, pos, ren, _] : seenView.each())
	{
		sf::Vector2f renderPosition = toScreenSpace(pos.tile, camera) + spatial::toXY(pos.offset);
		sf::IntRect uv = _textureAtlas->getUV(ren.texture, ren.uvSize, ren.uvCoords);
		_spriteLayers.at(ren.layer).addSprite(uv, SeenTileColor, renderPosition);
	}
}

void drft::system::EntityRenderer::batchEffectEntities(const CameraHandle& camera)
{
	const auto effectsView = _registry.view< const PositionComponent, const RenderComponent, const VisualEffectComponent, component::tag::InViewport>();
	for (auto const& [entity, pos, ren, effect] : effectsView.each())
	{
		if (effect.requiresInFOV && !_registry.all_of<component::tag::InPlayerFOV>(entity)) continue;

		sf::Vector2f renderPosition = toScreenSpace(pos.tile, camera) + spatial::toXY(pos.offset);
		sf::IntRect uv = _textureAtlas->getUV(ren.texture, ren.uvSize, ren.uvCoords);
		_spriteLayers.at(ren.layer).addSprite(uv, ren.color, renderPosition);
	}
}
