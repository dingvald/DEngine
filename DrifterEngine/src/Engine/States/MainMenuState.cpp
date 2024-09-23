#include "pch.h"
#include "MainMenuState.h"
#include "WorldMap/WorldMap.h"
#include <Engine/StateStack.h>


const std::filesystem::path SAVE_GAME_FILE_PATH = std::filesystem::current_path() / "data" / "savegame";

static const char* LayoutName = "Layout";

drft::MainMenuState::MainMenuState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui)
	: State(stack, context, gui)
{
	auto layout = tgui::VerticalLayout::create();
	layout->setOrigin(0.5f, 0.5f);
	layout->setSize("30%, 75%");
	layout->setPosition("50%, 50%");
	gui->add(layout, LayoutName);

	auto button_continue = tgui::Button::create();
	button_continue->setTextSize(32);
	button_continue->setText("Continue");
	button_continue->onPress([this]() { onContinue(); });

	auto button_new_game = tgui::Button::create();
	button_new_game->setTextSize(32);
	button_new_game->setText("New Game");
	button_new_game->onPress([this]() { onNewGame(); });

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
	layout->add(button_new_game);
	layout->addSpace(0.2f);
	layout->add(button_settings);
	layout->addSpace(0.2f);
	layout->add(button_exit);

	button_continue->setNavigationUp(button_exit);
	button_continue->setNavigationDown(button_new_game);

	button_new_game->setNavigationUp(button_continue);
	button_new_game->setNavigationDown(button_settings);

	button_settings->setNavigationUp(button_new_game);
	button_settings->setNavigationDown(button_exit);

	button_exit->setNavigationUp(button_settings);
	button_exit->setNavigationDown(button_continue);

	_gui->setNavigationDown(button_continue);
	_gui->setNavigationUp(button_continue);
}

bool drft::MainMenuState::handleEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackClear();
			return false;
		}
		/*
		if (ev.key.code == sf::Keyboard::Up || ev.key.code == sf::Keyboard::Numpad8)
		{
			onPressUp();
		}
		if (ev.key.code == sf::Keyboard::Down || ev.key.code == sf::Keyboard::Numpad2)
		{
			onPressDown();
		}
		*/
		break;
	}

	return false;
}

void drft::MainMenuState::onNewGame()
{
	std::filesystem::remove_all(SAVE_GAME_FILE_PATH);
	std::filesystem::create_directory(SAVE_GAME_FILE_PATH);
	requestStackClear();
	requestStackPush(States::Game);
}

void drft::MainMenuState::onContinue()
{
	requestStackClear();
	requestStackPush(States::Game);
}

void drft::MainMenuState::onSettings()
{
	// TODO: implement
}

void drft::MainMenuState::onExit()
{
	requestStackClear();
}

void drft::MainMenuState::onPressDown()
{
 	auto layout = _gui->get<tgui::VerticalLayout>(LayoutName);
	auto focused = layout->getFocusedChild();
	if (!focused)
	{
		const auto& children = layout->getWidgets();
		focused = children[0];
		focused->setFocused(true);
	}
	else
	{
		auto next = focused->getNavigationDown();
		next->setFocused(true);
	}
}

void drft::MainMenuState::onPressUp()
{
	auto layout = _gui->get<tgui::VerticalLayout>(LayoutName);
	auto focused = layout->getFocusedChild();
	if (!focused)
	{
		const auto& children = layout->getWidgets();
		focused = children[0];
		focused->setFocused(true);
	}
	else
	{
		auto next = focused->getNavigationUp();
		next->setFocused(true);
	}
}

bool drft::MainMenuState::hasSaveFile() const
{
	return std::filesystem::exists(".\\data\\savegame\\");
}
