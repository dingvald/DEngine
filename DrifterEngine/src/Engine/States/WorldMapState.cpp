#include "pch.h"
#include "WorldMapState.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "ProcGen/WorldGenerator.h"
#include "Random/RandomNumberGenerator.h"

static constexpr float FLASH_RATE = 1.0;

drft::WorldMapState::WorldMapState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();

	_mapBackground.setSize(VIEW.getSize());
	_mapBackground.setPosition(VIEW.getCenter());
	_mapBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,200)
		});

	_map.setTexture(getContext().textures.get("Sprites"));
	_generator = std::make_unique<gen::WorldGenerator>(rng::RandomNumberGenerator::getSeed());
}

bool drft::WorldMapState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		break;
	}

	return false;
}

bool drft::WorldMapState::update(const float dt)
{
	const float twoPI = 2.0f * 3.14152;
	const float f = 0.5f;
	static float elapsed = 0.0f;
	elapsed += dt;
	if (elapsed > 10.0)
	{
		elapsed = 0.0;
	}
	float alpha = 255 * ((std::sinf(twoPI * f * elapsed) + 1.f) / 2.f);

	_currentPositionTile.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));

	return false;
}

void drft::WorldMapState::render(sf::RenderTarget& target)
{
	_mapBackground.render(target);
	target.draw(_map);
	target.draw(_currentPositionTile);
}

void drft::WorldMapState::onPush()
{
	const auto& VIEW = getContext().window.getView();
	auto view = getContext().registry.view<component::Camera, component::Position>();
	for (auto [entity, camera, pos] : view.each())
	{
		_currentPosition = spatial::toChunkCoordinate(pos.position);
	}
	auto surroundings = spatial::getIntPointsInRadius(_currentPosition, 15);
	for (auto pos : surroundings)
	{
		auto biomeType = _generator->getBiomeType(pos);
		_map.addSprite(static_cast<unsigned int>(util::Sprite::Square),
			sf::Color::Black, sf::Vector2f((pos - _currentPosition) * spatial::TILE_WIDTH) + VIEW.getCenter());
		_map.addSprite(static_cast<unsigned int>(gen::BiomeSprites.at(biomeType.main)), 
			gen::BiomeColors.at(biomeType.main), sf::Vector2f((pos - _currentPosition) * spatial::TILE_WIDTH) + VIEW.getCenter());
	
	}
	_currentPositionTile.setSize({ spatial::TILE_WIDTH, spatial::TILE_HEIGHT });
	_currentPositionTile.setPosition((VIEW.getCenter()));
	_currentPositionTile.setFillColor(sf::Color::White);
}

void drft::WorldMapState::onPop()
{
	auto view = getContext().registry.view<component::action::OpenWorldMap>();
	for (auto entity : view)
	{
		getContext().registry.remove<component::action::OpenWorldMap>(entity);
	}
}
