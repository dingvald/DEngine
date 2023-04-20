#include "pch.h"
#include "GameOverState.h"

drft::GameOverState::GameOverState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();

	_gameOverWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,0),
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(150,0,0),
			.textSize = 32
			})
		.setTextString("You Died")
		.setTextOrigin(gui::ElementOrigin::BOTTOM_CENTER)
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

	static float elapsedTime = 0.0f;

	if (elapsedTime > 0.1)
	{
		auto& style = _gameOverWindow.modifyStyle(_gameOverWindow.getState());
		style.textSize = std::min(style.textSize += 4, 128);
		int alpha = style.fillColor.a;
		alpha += 4;
		style.fillColor.a = std::min(alpha, 255);
		elapsedTime = 0.0;
	}

	elapsedTime += dt;

	return true;
}

void drft::GameOverState::render(sf::RenderTarget& target)
{
	_gameOverWindow.render(target);
}
