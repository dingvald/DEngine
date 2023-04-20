#include "pch.h"
#include "GameOverState.h"

drft::GameOverState::GameOverState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();

	_gameOverWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,150),
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(150,0,0),
			.textSize = 64
			})
		.setTextString("You Died")
		.setTextOrigin(gui::ElementOrigin::CENTER)
		.setTextPosition(gui::ElementTextPosition::CENTER);
}

bool drft::GameOverState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackClear();
			requestStackPush(States::MainMenu);
			return false;
		}
		break;
	}
	return false;
}

bool drft::GameOverState::update(const float dt)
{
	_gameOverWindow.update(dt);

	return true;
}

void drft::GameOverState::render(sf::RenderTarget& target)
{
	_gameOverWindow.render(target);
}
