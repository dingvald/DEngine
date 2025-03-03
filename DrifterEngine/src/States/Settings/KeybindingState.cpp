#include "pch.h"
#include "KeybindingState.h"

#include <Utility/StringManipulation.h>

drft::KeybindingState::KeybindingState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	createKeybindingsGroup();
}

bool drft::KeybindingState::handleEvent(const sf::Event& ev)
{
	if (auto keyPressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (!_keybindingListener && keyPressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackPop();
			return true;
		}

		if (!_keybindingListener) return false;

		auto key = KeybindingUtils::getModifiedKey(keyPressed->scancode);
		return _keybindingListener->handleKeyPress(key);
	}

	return false;
}

bool drft::KeybindingState::update()
{
	if (_keybindingListener)
	{
		if (_keybindingListener->isDirty()) refreshKeybindingList(_keybindingList);
		if (_keybindingListener->isRequestingReset()) _keybindingListener.reset();
	}

	return false;
}

void drft::KeybindingState::createKeybindingsGroup()
{
	_keybindingList = tgui::PanelListBox::create();
	_keybindingList->setOrigin(0.5f, 0.5f);
	_keybindingList->setPosition({ "50%, 50%" });
	_keybindingList->setSize({ "50%, 50%" });
	_guiGroup->add(_keybindingList);

	auto title = tgui::Label::create();
	title->setText("Keybindings");
	title->setTextSize(64);
	title->setPosition({ tgui::bindLeft(_keybindingList), tgui::bindTop(_keybindingList) - title->getSize().y - 8});
	_guiGroup->add(title);

	// Setup Template
	auto panelTemplate = _keybindingList->getPanelTemplate();
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

	auto restoreDefaultsButton = tgui::Button::create();
	restoreDefaultsButton->setPosition({ "40%", tgui::bindBottom(_keybindingList) + 64});
	restoreDefaultsButton->setText("Restore Defaults");
	restoreDefaultsButton->setTextSize(24);
	restoreDefaultsButton->getRenderer()->setBorders({ 1, 1 });
	restoreDefaultsButton->getRenderer()->setBorderColor({ 100, 100, 100 });
	restoreDefaultsButton->onPress([this] { 
		getContext().keybindings.restoreDefaultKeybindings(); 
		refreshKeybindingList(_keybindingList);
		});
	_guiGroup->add(restoreDefaultsButton);

	// Populate list
	refreshKeybindingList(_keybindingList);
}

void drft::KeybindingState::refreshKeybindingList(tgui::PanelListBox::Ptr list)
{
	const unsigned int scrollAmount = list->getVerticalScrollbar()->getValue();
	list->removeAllItems();

	auto& keybindings = getContext().keybindings.getKeybindings();
	for (auto&& [stateName, stateBinding] : keybindings)
	{
		// State header
		auto stateLabel = list->addItem();
		stateLabel->remove(stateLabel->get("PrimaryKey"));
		stateLabel->remove(stateLabel->get("SecondaryKey"));
		auto stateNameText = stateLabel->get<tgui::Label>("ActionName");
		stateNameText->setText(drft::util::capitalize(stateName));
		stateNameText->setTextSize(32);
		stateNameText->setPosition("0%", "50%");

		// Keybindings inside state
		for (auto&& [actionName, keys] : stateBinding.getActionKeyPairs())
		{
			auto newEntry = list->addItem();

			auto name = newEntry->get<tgui::Label>("ActionName");
			name->setText(actionName);

			auto key1 = newEntry->get<tgui::Button>("PrimaryKey");
			key1->onPress([this, key1, &stateBinding, &actionName]() { onKeybindSlotPressed(key1, _guiGroup, stateBinding, actionName, BindingPosition::Primary); });
			if (keys.primary.key == sf::Keyboard::Scan::Unknown)
			{
				key1->setText("");
			}
			else
			{
				key1->setText(KeybindingUtils::convertModifiedKeyToString(keys.primary));
			}

			auto key2 = newEntry->get<tgui::Button>("SecondaryKey");
			key2->onPress([this, key2, &stateBinding, &actionName]() { onKeybindSlotPressed(key2, _guiGroup, stateBinding, actionName, BindingPosition::Secondary); });
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

	list->getVerticalScrollbar()->setValue(scrollAmount);
}

void drft::KeybindingState::onKeybindSlotPressed(tgui::Button::Ptr button, tgui::Group::Ptr keybindingsGroup, StateKeybindings& bindings, const std::string& actionName, BindingPosition bindingPosition)
{
	_keybindingListener.emplace(button, keybindingsGroup, bindings, actionName, bindingPosition);
}

drft::KeybindingState::KeybindingListener::KeybindingListener(
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
	_onFocusHandlerID = _button->onUnfocus([this] { onUnfocus(); });
}

drft::KeybindingState::KeybindingListener::~KeybindingListener()
{
	_button->setText(_keyName);
	_button->setFocused(false);
	_button->onUnfocus.disconnect(_onFocusHandlerID);
}

bool drft::KeybindingState::KeybindingListener::handleKeyPress(ModifiedKey key)
{
	if (key.key == sf::Keyboard::Scan::Unknown) return false; // Continue Listening

	switch (_state)
	{
	case State::Listening:
		return handleKeyInListeningState(key);
		break;
	case State::Conflict:
		return handleKeyInConflictState(key);
		break;
	case State::RequestReset:
		return handleKeyInRequestResetState(key);
		break;
	default:
		break;
	}
	return false;
}

bool drft::KeybindingState::KeybindingListener::isRequestingReset() const
{
	return _state == State::RequestReset;
}

bool drft::KeybindingState::KeybindingListener::isDirty() const
{
	return _isDirty;
}

void drft::KeybindingState::KeybindingListener::onUnfocus()
{
	_state = State::RequestReset;
}

void drft::KeybindingState::KeybindingListener::createBindingConflictPopup(ModifiedKey key, const std::string& conflictingActionName)
{
	auto backgroud = tgui::Panel::create();
	backgroud->setSize("100%", "100%");
	backgroud->getRenderer()->setBackgroundColor({ 10,10,10,70 });
	_group->add(backgroud, "PopupBackground");

	_popup = tgui::MessageBox::create();
	_group->add(_popup);

	_popup->setSize("15%", "12%");
	_popup->setPosition("50%", "50%");
	_popup->setOrigin(0.5f, 0.5f);
	_popup->setTitle("Keybinding Conflict");
	_popup->setTitleTextSize(16);
	_popup->setText(std::format("Key {} is bound to action:\n\n( {} )\n\nClear previous binding and continue?", 
		KeybindingUtils::convertModifiedKeyToString(key), 
		conflictingActionName));
	_popup->setTextSize(14);
	_popup->setLabelAlignment(tgui::HorizontalAlignment::Center);
	_popup->addButton("Yes");
	_popup->addButton("No");
	_popup->setButtonAlignment(tgui::MessageBox::Alignment::Center);

	_popup->getRenderer()->setBackgroundColor({ 20,20,20 });
	_popup->getRenderer()->setTitleBarHeight(26);
	_popup->getRenderer()->setTextColor({ 200, 200, 200 });
	_popup->getRenderer()->setTitleColor(tgui::Color::White);
	_popup->getRenderer()->setTitleBarColor({ 10, 10, 10 });

	_popup->onButtonPress([this, key, conflictingActionName](const tgui::String& button) {
		if (button == "Yes")
		{
			doSetBinding(key);
			_bindings.unbindKeyFromAction(key, conflictingActionName);
		}
		else if (button == "No")
		{
			_state = State::RequestReset;
		}
		closePopup();
	});
}

void drft::KeybindingState::KeybindingListener::closePopup()
{
	_group->remove(_popup);
	_group->remove(_group->get("PopupBackground"));
}

void drft::KeybindingState::KeybindingListener::doSetBinding(ModifiedKey key)
{
	_bindings.bindKeyToAction(key, _actionName, _bindingPosition);
	_keyName = KeybindingUtils::convertModifiedKeyToString(key);
	_state = State::RequestReset;
	_isDirty = true;
}

bool drft::KeybindingState::KeybindingListener::handleKeyInListeningState(ModifiedKey key)
{
	if (key.key == sf::Keyboard::Scan::Escape)
	{
		_state = State::RequestReset;
		return true;
	}

	auto action = _bindings.getActionForKey(key);
	if (action.has_value() && action.value() != _actionName)
	{
		createBindingConflictPopup(key, action.value());
		_state = State::Conflict;
		return true;
	}

	doSetBinding(key);
	_state = State::RequestReset;
	return true;
}

bool drft::KeybindingState::KeybindingListener::handleKeyInConflictState(ModifiedKey key)
{
	if (key.key == sf::Keyboard::Scan::Escape)
	{
		closePopup();
		_state = State::RequestReset;
		return true;
	}
	return false;
}

bool drft::KeybindingState::KeybindingListener::handleKeyInRequestResetState(ModifiedKey key)
{
	return handleKeyInListeningState(key);
}
