#include "pch.h"
#include "MainMenuState.h"
#include <Engine/StateStack.h>
#include <Engine/CommonEngineDirectories.h>


static const char* LayoutName = "Layout";

drft::MainMenuState::MainMenuState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	auto layout = tgui::GrowVerticalLayout::create();
	layout->setOrigin(0.5f, 0.5f);
	layout->setSize("30%, 75%");
	layout->setPosition("50%, 50%");
	layout->getRenderer()->setSpaceBetweenWidgets(32);
	_guiGroup->add(layout, LayoutName);

	auto button_continue = tgui::Button::create();
	button_continue->setTextSize(32);
	button_continue->setText("Continue");
	if (hasSaveFile())
	{
		button_continue->onPress([this]() { onContinueButton(); });
		button_continue->setFocused(true);
	}
	else
	{
		button_continue->getRenderer()->setTextColor(tgui::Color{ 100,100,100,255 });
		button_continue->setIgnoreMouseEvents(true);
		button_continue->setFocusable(false);
	}
	

	auto button_new_game = tgui::Button::create();
	button_new_game->setTextSize(32);
	button_new_game->setText("New Game");
	button_new_game->onPress([this]() { onNewGameButton(); });
	if (!button_continue->isFocusable()) button_new_game->setFocused(true);

	auto button_settings = tgui::Button::create();
	button_settings->setTextSize(32);
	button_settings->setText("Settings");
	button_settings->onPress([this]() { onSettingsButton(); });

	auto button_exit = tgui::Button::create();
	button_exit->setTextSize(32);
	button_exit->setText("Exit");
	button_exit->onPress([this]() { onExitButton(); });

	layout->add(button_continue);
	layout->add(button_new_game);
	layout->add(button_settings);
	layout->add(button_exit);

	button_continue->setNavigationUp(button_exit);
	button_continue->setNavigationDown(button_new_game);

	button_new_game->setNavigationUp(button_continue);
	button_new_game->setNavigationDown(button_settings);

	button_settings->setNavigationUp(button_new_game);
	button_settings->setNavigationDown(button_exit);

	button_exit->setNavigationUp(button_settings);
	button_exit->setNavigationDown(button_continue);
}

bool drft::MainMenuState::handleEvent(const sf::Event& ev)
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

void drft::MainMenuState::onNewGameButton()
{
	std::filesystem::remove_all(SAVE_DIRECTORY);
	std::filesystem::create_directory(SAVE_DIRECTORY);
	requestStackClear();
	requestStackPush(States::Game);
}

void drft::MainMenuState::onContinueButton()
{
	requestStackClear();
	requestStackPush(States::Game);
}

void drft::MainMenuState::onSettingsButton()
{
	requestStackPush(States::Settings);
}

void drft::MainMenuState::onExitButton()
{
	requestStackClear();
}

bool drft::MainMenuState::hasSaveFile() const
{
	return std::filesystem::exists(PLAYER_SAVE_FILE_PATH);
}
