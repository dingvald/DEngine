#include "pch.h"
#include "GameOverState.h"

drft::GameOverState::GameOverState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	auto background = tgui::Panel::create();
	background->setSize("100%", "100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 255, 0, 0, 50 });

	auto label = tgui::Label::create();
	label->setTextSize(32);
	label->setText("You Died");
	label->setPosition("50%", "50%");
	label->setOrigin(0.5f, 0.5f);
	label->getRenderer()->setTextColor(tgui::Color::White);

	_guiGroup->add(background);
	_guiGroup->add(label);
}

bool drft::GameOverState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackClear();
			return true;
		}
	}
	return false;
}

bool drft::GameOverState::update()
{
	return true;
}
