#include "pch.h"
#include "SettingsState.h"
#include <Engine/StateStack.h>

static const char* SettingsListName = "SettingsList";

drft::SettingsState::SettingsState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	registerSubStates(stack);
	
	auto background = tgui::Panel::create();
	background->setSize("100%, 100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 255 });

	auto settings_list = tgui::VerticalLayout::create();
	settings_list->setOrigin(0.5f, 0.5f);
	settings_list->setSize("30%, 75%");
	settings_list->setPosition("50%, 50%");

	_guiGroup->add(background);
	_guiGroup->add(settings_list, SettingsListName);

	auto button_keybindings = tgui::Button::create();
	button_keybindings->setTextSize(32);
	button_keybindings->setText("Keybindings");
	button_keybindings->onPress([this]() { requestStackPush(States::Keybindings); });

	settings_list->add(button_keybindings);
}

bool drft::SettingsState::handleEvent(const sf::Event& ev)
{
	if (auto keyPressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackPop();
			return true;
		}
	}
		
	return false;
}

void drft::SettingsState::registerSubStates(StateStack& stack)
{
	
}
