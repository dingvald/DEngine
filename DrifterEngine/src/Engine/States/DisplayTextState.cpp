#include "pch.h"
#include "DisplayTextState.h"
#include "Components/Components.h"
#include "Spatial/Helpers.h"
#include "Spatial/Conversions.h"
#include "ProcGen/WorldGeneration/WorldGenerator.h"

drft::DisplayTextState::DisplayTextState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();
	_background.setSize(VIEW.getSize());
	_background.setPosition(VIEW.getCenter());
	_background.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,200)
		});

	_sprites.setTexture(getContext().textures.get("Sprites"));
}

bool drft::DisplayTextState::handleEvent(const sf::Event& ev)
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
	default:
		return false;
		break;
	}

	return false;
}

bool drft::DisplayTextState::update(const float dt)
{
	return false;
}

void drft::DisplayTextState::render(sf::RenderTarget& target)
{
	_background.render(target);
	_display.render(target);
	target.draw(_sprites);
}

void drft::DisplayTextState::onPush()
{
	getSessionParameters();
	determineDisplayType();
	buildDisplay();
}

void drft::DisplayTextState::onPop()
{

}

void drft::DisplayTextState::getSessionParameters()
{
	auto displayView = getContext().registry.view<component::action::DisplayText>();
	for (auto [entity, display] : displayView.each())
	{
		_sessionParameters = display.parameters;
		getContext().registry.remove<component::action::DisplayText>(entity);
	}
}

void drft::DisplayTextState::determineDisplayType()
{
	if (_sessionParameters.at("Type").compare("map") == 0)
	{
		_displayType = DisplayType::Map;
	}
	else if (_sessionParameters.at("Type").compare("text") == 0)
	{
		_displayType = DisplayType::Text;
	}
}

void drft::DisplayTextState::buildDisplay()
{
	switch (_displayType)
	{
		case DisplayType::Map:
		{
			buildMap();
		}
		break;
		case DisplayType::Text:
		{
			buildText();
		}
		break;
	}
}

void drft::DisplayTextState::buildMap()
{
	
}

void drft::DisplayTextState::buildText()
{

}
