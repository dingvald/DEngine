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
	createKeybindingsGroup();

	showSettingsList();
}

bool drft::SettingsState::handleEvent(const sf::Event& ev)
{
	if (auto keyPressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Escape)
		{
			if (_keybindingListener.has_value())
			{
				_keybindingListener.reset();
				return true;
			}
			else if (_isShowingSettingsList)
			{
				requestStackPop();
				return true;
			}
			else
			{
				showSettingsList();
				return true;
			}
		}

		if (_keybindingListener.has_value())
		{
			auto key = KeybindingUtils::getModifiedKey(keyPressed->scancode);
			if (_keybindingListener->handleKeyPress(key))
			{
				_keybindingListener.reset();
			}
			return true;
		}
	}
	if (_keybindingListener && _keybindingListener->shouldBeReset())
	{
		_keybindingListener.reset();
	}
		
	return false;
}

void drft::SettingsState::showSettingsList()
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
	button_keybindings->onPress([this]() { showSettingSelection(_keybindingsGroup); });

	settings_list->add(button_visuals);
	settings_list->add(button_audio);
	settings_list->add(button_keybindings);
}

void drft::SettingsState::createKeybindingsGroup()
{
	_keybindingsGroup = tgui::Group::create();
	_guiGroup->add(_keybindingsGroup);

	auto keybindingList = tgui::PanelListBox::create();
	keybindingList->setOrigin(0.5f, 0.5f);
	keybindingList->setPosition({ "50%, 50%" });
	keybindingList->setSize({ "50%, 50%" });
	_keybindingsGroup->add(keybindingList);
	// Setup Template
	auto panelTemplate = keybindingList->getPanelTemplate();
	panelTemplate->setWidth("100%");

	auto actionNameLabel = tgui::Label::create();
	actionNameLabel->setOrigin(0.f, 0.5f);
	actionNameLabel->setPosition("2%", "50%");
	actionNameLabel->setTextSize(16);
	panelTemplate->add(actionNameLabel, "ActionName");

	auto primaryKey = tgui::Button::create();
	primaryKey->setOrigin(0.f, 0.5f);
	primaryKey->setPosition("40%", "50%");
	primaryKey->setSize("20%", "100%");
	primaryKey->setTextSize(16);
	primaryKey->getRenderer()->setBorders({ 1, 1 });
	primaryKey->getRenderer()->setBorderColor({ 100,100,100,100 });
	panelTemplate->add(primaryKey, "PrimaryKey");

	auto secondaryKey = tgui::Button::create();
	secondaryKey->setOrigin(0.f, 0.5f);
	secondaryKey->setPosition("65%", "50%");
	secondaryKey->setSize("20%", "100%");
	secondaryKey->setTextSize(16);
	secondaryKey->getRenderer()->setBorders({ 1, 1 });
	secondaryKey->getRenderer()->setBorderColor({ 100,100,100,100 });
	panelTemplate->add(secondaryKey, "SecondaryKey");

	// Populate list
	auto& keybindings = getContext().keybindings.getKeybindings();
	for (auto&& [stateName, stateBinding] : keybindings)
	{
		// State header
		auto stateLabel = keybindingList->addItem();
		stateLabel->remove(stateLabel->get("PrimaryKey"));
		stateLabel->remove(stateLabel->get("SecondaryKey"));
		auto stateNameText = stateLabel->get<tgui::Label>("ActionName");
		stateNameText->setText(drft::util::capitalize(stateName));
		stateNameText->setTextSize(32);
		stateNameText->setPosition("0%", "50%");

		// Keybindings inside state
		for (auto&& [actionName, keys] : stateBinding.getActionKeyPairs())
		{
			auto newEntry = keybindingList->addItem();

			auto name = newEntry->get<tgui::Label>("ActionName");
			name->setText(actionName);

			auto key1 = newEntry->get<tgui::Button>("PrimaryKey");
			key1->onPress([this, key1, &stateBinding, &actionName]() { onKeybindSlotPressed(key1, _keybindingsGroup, stateBinding, actionName, BindingPosition::Primary); });
			if (keys.primary.key == sf::Keyboard::Scan::Unknown)
			{
				key1->setText("");
			}
			else
			{
				key1->setText(KeybindingUtils::convertModifiedKeyToString(keys.primary));
			}

			auto key2 = newEntry->get<tgui::Button>("SecondaryKey");
			key2->onPress([this, key2, &stateBinding, &actionName]() { onKeybindSlotPressed(key2, _keybindingsGroup, stateBinding, actionName, BindingPosition::Secondary); });
			if (keys.secondary.key == sf::Keyboard::Scan::Unknown)
			{
				key2->setText("");
			}
			else
			{
				key2->setText(KeybindingUtils::convertModifiedKeyToString(keys.secondary));
			}	
		}
	}
}

void drft::SettingsState::onKeybindSlotPressed(tgui::Button::Ptr button, tgui::Group::Ptr keybindingsGroup, StateKeybindings& bindings, const std::string& actionName, BindingPosition bindingPosition)
{
	_keybindingListener.emplace(button, keybindingsGroup, bindings, actionName, bindingPosition);
}

drft::SettingsState::KeybindingListener::KeybindingListener(
	tgui::Button::Ptr button,
	tgui::Group::Ptr group,
	StateKeybindings& bindings, 
	const std::string& actionName, 
	BindingPosition bindingPosition)
	: _button(button)
	, _group(group)
	, _bindings(bindings)
	, _actionName(actionName)
	, _bindingPosition(bindingPosition)
{
	_keyName = _button->getText().toStdString();
	_button->setText("<Enter New Key>");
	_button->onUnfocus([this] { onUnfocus(); });
}

drft::SettingsState::KeybindingListener::~KeybindingListener()
{
	_button->setText(_keyName);
	_button->setFocused(false);
	_button->onUnfocus.disconnectAll();
}

bool drft::SettingsState::KeybindingListener::shouldBeReset() const
{
	return _requestReset;
}

bool drft::SettingsState::KeybindingListener::handleKeyPress(ModifiedKey key)
{
	if (key.key == sf::Keyboard::Scan::Unknown) return false;
	
	auto action = _bindings.getActionForKey(key);
	if (action.has_value())
	{
		if (action.value() == _actionName) return true;

		createBindingConflictPopup(action.value());
		return false;
	}

	doSetBinding(key);
	return true;
}

void drft::SettingsState::KeybindingListener::onUnfocus()
{
	_requestReset = true;
}

void drft::SettingsState::KeybindingListener::createBindingConflictPopup(const std::string& conflictingActionName)
{
	auto backgroud = tgui::Panel::create();
	backgroud->setSize("100%", "100%");
	backgroud->getRenderer()->setBackgroundColor({ 100,100,100,100 });
	_group->add(backgroud);

	auto messageBox = tgui::MessageBox::create();
	_group->add(messageBox);

	messageBox->setSize("15%", "10%");
	messageBox->setPosition("50%", "50%");
	messageBox->setOrigin(0.5f, 0.5f);
	messageBox->setTitle("Keybinding Conflict");
	messageBox->setTitleTextSize(16);
	messageBox->setText(std::format("This key is bound to action:\n{}\nClear previous binding and continue?", conflictingActionName));
	messageBox->setLabelAlignment(tgui::HorizontalAlignment::Center);
	messageBox->addButton("Yes");
	messageBox->addButton("No");
	messageBox->setButtonAlignment(tgui::MessageBox::Alignment::Center);
	messageBox->getRenderer()->setTitleBarColor(tgui::Color::Black);
}

void drft::SettingsState::KeybindingListener::doSetBinding(ModifiedKey key)
{
	_bindings.bindKeyToAction(key, _actionName, _bindingPosition);
	_keyName = KeybindingUtils::convertModifiedKeyToString(key);
}
