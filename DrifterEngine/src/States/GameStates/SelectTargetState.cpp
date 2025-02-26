#include "pch.h"
#include "SelectTargetState.h"

#include "Components/Components.h"
#include "Components/PositionComponent.h"
#include "Components/RenderComponent.h"

#include "Events/SendFloatingMessageEvent.h"
#include <Keybindings/Keybindings.h>

#include <Spatial/Conversions.h>
#include "Spatial/Helpers.h"
#include "Spatial/Grid2d.h"
#include "Systems/Helpers/SpawnEffect.h"

#include "Utility/SpriteOptions.h"

static const sf::Color DEFAULT_TARGET_RANGE_COLOR = sf::Color(0, 0, 100, 100);
static const sf::Color DEFAULT_TARGET_AOE_COLOR = sf::Color(50, 50, 200, 150);
static const sf::Color TARGET_AOE_OUT_OF_RANGE_COLOR = sf::Color(255, 0, 0, 100);

drft::SelectTargetState::SelectTargetState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	_displayText.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100),
			.innerPadding = {4.f, 4.f},
			.font = &context.fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 16
		});
	_displayText.setTextString("Select target");
	_displayText.setOrigin(gui::ElementPosition::CENTER);
	_displayText.setPosition(context.window.getView().getCenter() + sf::Vector2f(8.f, -64.f));

	_actionMap.bindAction("move_south_west",	[this]() { moveCursor({ -1, 1 }); });
	_actionMap.bindAction("move_south",			[this]() { moveCursor({ 0, 1 }); });
	_actionMap.bindAction("move_south_east",	[this]() { moveCursor({ 1, 1 }); });
	_actionMap.bindAction("move_west",			[this]() { moveCursor({ -1, 0 }); });
	_actionMap.bindAction("move_east",			[this]() { moveCursor({ 1, 0 }); });
	_actionMap.bindAction("move_north_west",	[this]() { moveCursor({ -1, -1 }); });
	_actionMap.bindAction("move_north",			[this]() { moveCursor({ 0, -1 }); });
	_actionMap.bindAction("move_north_east",	[this]() { moveCursor({ 1, -1 }); });

	_actionMap.bindAction("interact", [this]() { select(); });
}

bool drft::SelectTargetState::handleEvent(const sf::Event& ev)
{;
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return true;
		}

		const Keybindings& keybindings = getContext().keybindings;
		const ModifiedKey key = KeybindingUtils::getModifiedKey(ev.key.scancode);
		auto action = keybindings["simulation"].getActionForKey(key);
		if (action)
		{
			_actionMap.callAction(action.value());
			return true;
		}
		break;
	}
	return false;
}

bool drft::SelectTargetState::update()
{
	return true;
}

void drft::SelectTargetState::render(sf::RenderTarget& target)
{
	_displayText.render(target);
}

void drft::SelectTargetState::onPush()
{
	auto selectTargetView = getContext().registry.view<component::action::SelectTarget>();
	if (auto selectTarget = getContext().registry.try_get<component::action::SelectTarget>(selectTargetView.front()))
	{
		_targetSelect = selectTarget;
	}
	else
	{
		throw std::exception("No entities are trying to select a target... why are you here?");
	}

	if (auto startPos = getContext().registry.try_get<PositionComponent>(selectTargetView.front()))
	{
		_startPosition = startPos->tile;
		_cursorPosition = _startPosition;
	}
	else
	{
		requestStackPop();
	}

	const entt::id_type tileSetTexture = entt::hashed_string("rectangle").value();

	auto radius = spatial::getIntCircleInRadius(_startPosition, _targetSelect->range.getMax());

	SpriteOptions radiusEffect = { 
		.uvCoords = sf::Vector2i{0, 0}, 
		.texture = tileSetTexture, 
		.uvSize = DefaultTileTextureSize, 
		.layer = static_cast<unsigned int>(system::RenderLayer::Tiles), 
		.color = DEFAULT_TARGET_RANGE_COLOR
	};
	for (auto&& tile : radius)
	{
		auto effect = system::spawnEffect(getContext().registry,
			{
				.frames = {radiusEffect},
				.position = spatial::asTileSpace(tile),
				.ttl = -1
			});
		_radiusEffects.push_back(effect);
	}

	SpriteOptions aoeEffect = {
		.uvCoords = sf::Vector2i{0, 0}, 
		.texture = tileSetTexture, 
		.uvSize = DefaultTileTextureSize, 
		.layer = static_cast<unsigned int>(system::RenderLayer::Tiles), 
		.color = DEFAULT_TARGET_AOE_COLOR };
	for (auto&& tile : _targetSelect->targetShape)
	{
		auto tile3d = spatial::vec3FromPlanar(tile);
		auto effect = system::spawnEffect(getContext().registry,
			{
				.frames = { aoeEffect },
				.position = _startPosition + spatial::asTileSpace(tile3d),
				.ttl = -1,
				.requiresInFOV = false
			});
		_aoeEffects.push_back(effect);
	}

	SpriteOptions cursorEffect = { 
		.uvCoords = sf::Vector2i{0, 0}, 
		.texture = tileSetTexture, 
		.uvSize = DefaultTileTextureSize,
		.layer = static_cast<unsigned int>(system::RenderLayer::Tiles), 
		.color = sf::Color{255, 255, 200, 150} };
	_cursor = system::spawnEffect(getContext().registry,
		{
			.frames = {cursorEffect},
			.position = _startPosition,
			.ttl = -1,
			.requiresInFOV = false
		});
}

void drft::SelectTargetState::onPop()
{
	getContext().registry.clear<component::action::SelectTarget>();
	getContext().registry.destroy(_cursor);
	for (auto radiusEffect : _radiusEffects)
	{
		getContext().registry.destroy(radiusEffect);
	}
	for (auto aoeEffect : _aoeEffects)
	{
		getContext().registry.destroy(aoeEffect);
	}
}

bool drft::SelectTargetState::isInRange() const
{
	return spatial::distance3d(_cursorPosition, _startPosition) <= _targetSelect->range.getMax();
}

void drft::SelectTargetState::moveCursor(sf::Vector2i direction)
{
	sf::Vector3i dir = { direction.x, direction.y, 0 };
	for (auto effect : _aoeEffects)
	{
		const auto& pos = getContext().registry.patch<PositionComponent>(effect,
			[dir](PositionComponent& pos)
			{
				pos.tile += dir;
			});

		auto& render = getContext().registry.get<RenderComponent>(effect);
		if (spatial::distance3d(pos.tile, _startPosition) <= _targetSelect->range.getMax())
		{
			render.color = DEFAULT_TARGET_AOE_COLOR;
		}
		else
		{
			render.color = TARGET_AOE_OUT_OF_RANGE_COLOR;
		}
	}

	const auto& pos = getContext().registry.patch<PositionComponent>(_cursor,
		[dir](PositionComponent& pos)
		{
			pos.tile += dir;
		});

	_cursorPosition += dir;
}

void drft::SelectTargetState::select()
{
	if (!isInRange())
	{
		const sf::Vector2f messagePosition = spatial::toXY(spatial::toFloatSpace(_startPosition));
		auto& dispatcher = getContext().registry.ctx().get<entt::dispatcher&>();

		dispatcher.trigger(events::SendFloatingMessageEvent{
			.message = "Out of range",
			.color = sf::Color::Red,
			.position = messagePosition,
			.velocity = {0.f,-0.1f},
			.isScreenSpace = false,
			.ttl = 80
			});
	}
	else
	{
		auto selectTargetView = getContext().registry.view<component::action::SelectTarget>();
		if (auto selectTarget = getContext().registry.try_get<component::action::SelectTarget>(selectTargetView.front()))
		{
			selectTarget->onTargetSelect(_cursorPosition);
		}
		requestStackPop();
	}
}
