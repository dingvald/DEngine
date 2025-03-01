#include "pch.h"
#include "PauseState.h"


drft::PauseState::PauseState(StateStack& stack, StateContext& context)
    :State(stack, context)
{
	auto background = tgui::Panel::create();
	background->setSize("100%, 100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 100 });

	auto layout = tgui::VerticalLayout::create();
	layout->setOrigin(0.5f, 0.5f);
	layout->setSize("30%, 75%");
	layout->setPosition("50%, 50%");

	_guiGroup->add(background);
	_guiGroup->add(layout);

	auto button_continue = tgui::Button::create();
	button_continue->setTextSize(32);
	button_continue->setText("Continue");
	button_continue->onPress([this]() { onContinueButton(); });

	auto button_settings = tgui::Button::create();
	button_settings->setTextSize(32);
	button_settings->setText("Settings");
	button_settings->onPress([this]() { onSettingsButton(); });

	auto button_exit = tgui::Button::create();
	button_exit->setTextSize(32);
	button_exit->setText("Exit");
	button_exit->onPress([this]() { onExitButton(); });

	layout->add(button_continue);
	layout->addSpace(0.2f);
	layout->add(button_settings);
	layout->addSpace(0.2f);
	layout->add(button_exit);

	button_continue->setNavigationUp(button_exit);
	button_continue->setNavigationDown(button_settings);

	button_settings->setNavigationUp(button_continue);
	button_settings->setNavigationDown(button_exit);

	button_exit->setNavigationUp(button_settings);
	button_exit->setNavigationDown(button_continue);
}

bool drft::PauseState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			onContinueButton();
			return true;
		}
	}
	return false;
}

void drft::PauseState::onContinueButton()
{
	requestStackPop();
}

void drft::PauseState::onSettingsButton()
{
	requestStackPush(States::Settings);
}

void drft::PauseState::onExitButton()
{
	requestStackClear();
	requestStackPush(States::MainMenu);
}
