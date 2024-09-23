#include "pch.h"
#include "GameOverState.h"

drft::GameOverState::GameOverState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui)
	: State(stack, context, gui)
{
	const auto& VIEW = getContext().window.getView();

	_gameOverPanel.setSize({ VIEW.getSize().x, VIEW.getSize().y / 4 })
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,0),
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(150,0,0),
			.textSize = 32,
			.textScale = {0.5, 0.5}
			})
		.setTextString("YOU DIED")
		.setTextOrigin(gui::ElementPosition::CENTER)
		.setTextPosition(gui::ElementPosition::CENTER);
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
	_gameOverPanel.update(dt);

	static float elapsedTime = 0.0f;
	if (elapsedTime > 0.1)
	{
		auto& style = _gameOverPanel.modifyStyle(_gameOverPanel.getState());
		style.textScale.x = std::min(style.textScale.x + 0.1f, 3.f);
		style.textScale.y = std::min(style.textScale.y + 0.1f, 3.f);
		int alpha = style.fillColor.a;
		alpha += 6;
		style.fillColor.a = std::min(alpha, 180);
		elapsedTime = 0.0;
	}

	elapsedTime += dt;

	return true;
}

void drft::GameOverState::render(sf::RenderTarget& target)
{
	_gameOverPanel.render(target);
}
