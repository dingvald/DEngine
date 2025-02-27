#include "pch.h"
#include "SettingsState.h"
#include <Engine/StateStack.h>

#include <Utility/StandardLogger.h>

drft::SettingsState::SettingsState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	auto background = tgui::Panel::create();
	background->setSize("100%, 100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 255 });

	_guiGroup->add(background);

	createSettingsListGroup();
	createKeybindingsGroup();
}

bool drft::SettingsState::handleEvent(const sf::Event& ev)
{
	if (auto keyPressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Escape)
		{
			if (_isShowingSettingsList)
			{
				requestStackPop();
				return true;
			}
			else
			{
				returnToSettingsList();
				return true;
			}
		}
	}
		
	return false;
}

void drft::SettingsState::returnToSettingsList()
{
	// Hide all other groups
	_keybindingsGroup->setVisible(false);

	////////////////////////

	_settingsListGroup->setVisible(true);

	_isShowingSettingsList = true;
}

void drft::SettingsState::showSettingSelection(tgui::Group::Ptr group)
{
	group->setVisible(true);
	_settingsListGroup->setVisible(false);

	_isShowingSettingsList = false;
}

void drft::SettingsState::createSettingsListGroup()
{
	_settingsListGroup = tgui::Group::create();
	_guiGroup->add(_settingsListGroup);

	auto settings_list = tgui::GrowVerticalLayout::create();
	_settingsListGroup->add(settings_list);

	settings_list->setOrigin(0.5f, 0.5f);
	settings_list->setSize("30%, 75%");
	settings_list->setPosition("50%, 50%");

	auto button_visuals = tgui::Button::create();
	button_visuals->setTextSize(32);
	button_visuals->setText("Visuals");
	button_visuals->onPress([this]() { warning_logger << "Visuals settings not yet implmented" << std::endl; });

	auto button_audio = tgui::Button::create();
	button_audio->setTextSize(32);
	button_audio->setText("Audio");
	button_audio->onPress([this]() { warning_logger << "Audio settings not yet implmented" << std::endl; });

	auto button_keybindings = tgui::Button::create();
	button_keybindings->setTextSize(32);
	button_keybindings->setText("Keybindings");
	button_keybindings->onPress([this]() { showSettingSelection(_keybindingsGroup); });

	settings_list->add(button_visuals);
	settings_list->add(button_audio);
	settings_list->add(button_keybindings);
}

void drft::SettingsState::createKeybindingsGroup()
{
	_keybindingsGroup = tgui::Group::create();
	_guiGroup->add(_keybindingsGroup);

	auto label = tgui::Label::create("test");
	label->setPosition("50%, 50%");
	label->setOrigin("50%, 50%");

	_keybindingsGroup->add(label);
}
