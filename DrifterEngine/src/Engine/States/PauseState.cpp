#include "pch.h"
#include "PauseState.h"

static const char* LayoutName = "Layout";

drft::PauseState::PauseState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui)
    :State(stack, context, gui)
{
	auto background = tgui::Panel::create();
	background->setSize("100%, 100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 100 });

	auto layout = tgui::VerticalLayout::create();
	layout->setOrigin(0.5f, 0.5f);
	layout->setSize("30%, 75%");
	layout->setPosition("50%, 50%");

	gui->add(background);
	gui->add(layout, LayoutName);

	auto button_continue = tgui::Button::create();
	button_continue->setTextSize(32);
	button_continue->setText("Continue");
	button_continue->onPress([this]() { onContinue(); });

	auto button_settings = tgui::Button::create();
	button_settings->setTextSize(32);
	button_settings->setText("Settings");
	button_settings->onPress([this]() { onSettings(); });

	auto button_exit = tgui::Button::create();
	button_exit->setTextSize(32);
	button_exit->setText("Exit");
	button_exit->onPress([this]() { onExit(); });

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

	_gui->setNavigationUp(button_continue);
	_gui->setNavigationDown(button_continue);
}

bool drft::PauseState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
	}

	return false;
}

void drft::PauseState::onContinue()
{
	requestStackPop();
}

void drft::PauseState::onSettings()
{
	// TODO: implement
}

void drft::PauseState::onExit()
{
	requestStackClear();
	requestStackPush(States::MainMenu);
}
