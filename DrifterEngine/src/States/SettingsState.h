#pragma once
#include <States/State.h>
#include <Keybindings/Keybindings.h>

class StateKeybindings;

namespace drft
{
	class SettingsState : public State
	{
	public:
		SettingsState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);

	private:
		class KeybindingListener
		{
		public:
			KeybindingListener(tgui::Button::Ptr button, tgui::Group::Ptr keybindingsGroup, StateKeybindings& bindings, const std::string& actionName, BindingPosition bindingPosition);
			~KeybindingListener();

			bool shouldBeReset() const;
			bool handleKeyPress(ModifiedKey key);

		private:
			void onUnfocus();
			void createBindingConflictPopup(const std::string& conflictingActionName);
			void doSetBinding(ModifiedKey key);

		private:
			bool _requestReset = false;
			tgui::Button::Ptr _button;
			tgui::Group::Ptr _group;
			StateKeybindings& _bindings;
			const std::string& _actionName;
			BindingPosition _bindingPosition;
			std::string _keyName;
		};

	private:
		void showSettingsList();
		void showSettingSelection(tgui::Group::Ptr group);

		void createSettingsListGroup();
		void createKeybindingsGroup();

		void onKeybindSlotPressed(tgui::Button::Ptr button, tgui::Group::Ptr keybindingsGroup, StateKeybindings& bindings, const std::string& actionName, BindingPosition bindingPosition);

	private:
		tgui::Group::Ptr _settingsListGroup;
		tgui::Group::Ptr _keybindingsGroup;

		bool _isShowingSettingsList = true;
		std::optional<KeybindingListener> _keybindingListener;
	};
}