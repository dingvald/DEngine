#include "pch.h"
#include "SettingsState.h"
#include <Engine/StateStack.h>

#include <Utility/StandardLogger.h>
#include <Utility/StringManipulation.h>

drft::SettingsState::SettingsState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	auto background = tgui::Panel::create();
	background->setSize("100%, 100%");
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 255 });

	_guiGroup->add(background);

	createSettingsListGroup();
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

void drft::SettingsState::createSettingsListGroup()
{
	auto settings_list = tgui::GrowVerticalLayout::create();
	_guiGroup->add(settings_list);

	settings_list->setOrigin(0.5f, 0.5f);
	settings_list->setSize("30%, 75%");
	settings_list->setPosition("50%, 50%");
	settings_list->getRenderer()->setSpaceBetweenWidgets(32);

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
	button_keybindings->onPress([this]() { requestStackPush(States::Keybindings); });

	settings_list->add(button_visuals);
	settings_list->add(button_audio);
	settings_list->add(button_keybindings);
}
