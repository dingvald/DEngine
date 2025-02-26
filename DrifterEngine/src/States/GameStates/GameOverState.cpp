#include "pch.h"
#include "GameOverState.h"

drft::GameOverState::GameOverState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	auto background = tgui::Panel::create();
	background->setSize("100%, 100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 100 });

	auto label = tgui::Label::create();
	label->setTextSize(32);
	label->setText("You Died");
	label->setPosition("50%, 50%");
	label->setOrigin("50%, 50%");

	_guiGroup->add(background);
	_guiGroup->add(label);
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
}

void drft::GameOverState::render(sf::RenderTarget& target)
{
}
