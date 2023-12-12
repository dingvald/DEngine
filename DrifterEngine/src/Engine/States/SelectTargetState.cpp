#include "pch.h"
#include "SelectTargetState.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"
#include "Spatial/Grid.h"
#include "Systems/Helpers/SpawnEffect.h"

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
}

bool drft::SelectTargetState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Numpad8)
		{
			moveCursor(sf::Vector2i(0, -1));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad2)
		{
			moveCursor(sf::Vector2i(0, 1));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad6)
		{
			moveCursor(sf::Vector2i(1, 0));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad4)
		{
			moveCursor(sf::Vector2i(-1, 0));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad7)
		{
			moveCursor(sf::Vector2i(-1, -1));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad9)
		{
			moveCursor(sf::Vector2i(1, -1));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad3)
		{
			moveCursor(sf::Vector2i(1, 1));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad1)
		{
			moveCursor(sf::Vector2i(-1, 1));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad5)
		{
			moveCursor(sf::Vector2i(0, 0));
			return false;
		}
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			auto selectTargetView = getContext().registry.view<component::action::SelectTarget>();
			if (auto selectTarget = getContext().registry.try_get<component::action::SelectTarget>(selectTargetView.front()))
			{
				selectTarget->onTargetSelect(_cursorPosition);
			}
			requestStackPop();
		}
		break;
	}
	return false;
}

bool drft::SelectTargetState::update(const float dt)
{
	return true;
}

void drft::SelectTargetState::render(sf::RenderTarget& target)
{
	renderTargetRadius(target);
	renderTargetAoE(target);
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

	if (auto startPos = getContext().registry.try_get<component::Position>(selectTargetView.front()))
	{
		_startPosition = startPos->position;
		_cursorPosition = _startPosition;
	}
	else
	{
		requestStackPop();
	}

	auto radius = spatial::getIntCircleInRadius(_startPosition, _targetSelect->range.max);
	for (auto&& tile : radius)
	{
		auto effect = system::spawnEffect(getContext().registry,
			{
				.color = DEFAULT_TARGET_RANGE_COLOR,
				.sprites = {4},
				.layer = system::RenderLayer::Tiles,
				.position = tile,
				.ttl = -1
			});
		_radiusEffects.push_back(effect);
	}
	for (auto&& tile : _targetSelect->targetShape)
	{
		auto effect = system::spawnEffect(getContext().registry,
			{
				.color = DEFAULT_TARGET_AOE_COLOR,
				.sprites = {4},
				.layer = system::RenderLayer::Tiles,
				.position = tile + _startPosition,
				.ttl = -1,
				.requiresInFOV = false
			});
		_aoeEffects.push_back(effect);
	}
	_cursor = system::spawnEffect(getContext().registry,
		{
			.color = sf::Color(255,255,204,100),
				.sprites = {83},
				.layer = system::RenderLayer::EffectsFront,
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

void drft::SelectTargetState::moveCursor(sf::Vector2i direction)
{
	for (auto effect : _aoeEffects)
	{
		const auto& pos = getContext().registry.patch<component::Position>(effect,
			[direction](component::Position& pos)
			{
				pos.position += direction;
			});
		auto& render = getContext().registry.get<component::Render>(effect);
		if (spatial::distance(pos.position, _startPosition) > _targetSelect->range.max)
		{
			render.color = TARGET_AOE_OUT_OF_RANGE_COLOR;
		}
		else
		{
			render.color = DEFAULT_TARGET_AOE_COLOR;
		}
	}
	const auto& pos = getContext().registry.patch<component::Position>(_cursor,
		[direction](component::Position& pos)
		{
			pos.position += direction;
		});
	_cursorPosition += direction;
}

void drft::SelectTargetState::renderTargetRadius(sf::RenderTarget& target)
{

}

void drft::SelectTargetState::renderTargetAoE(sf::RenderTarget& target)
{

}
