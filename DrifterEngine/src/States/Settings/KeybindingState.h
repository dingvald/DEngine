#pragma once

#pragma once
#include <States/State.h>
#include <Keybindings/Keybindings.h>

class StateKeybindings;

namespace drft
{
	class KeybindingState : public State
	{
	public:
		KeybindingState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);
		bool update() override;

	private:
		class KeybindingListener
		{
		public:
			enum class State
			{
				Listening,
				Conflict,
				RequestReset
			};
		public:
			KeybindingListener(tgui::Button::Ptr button, tgui::Group::Ptr keybindingsGroup, StateKeybindings& bindings, const std::string& actionName, BindingPosition bindingPosition);
			~KeybindingListener();

			bool handleKeyPress(ModifiedKey key);
			bool isRequestingReset() const;
			bool isDirty() const;

		private:
			void onUnfocus();
			void createBindingConflictPopup(ModifiedKey key, const std::string& conflictingActionName);
			void closePopup();
			void doSetBinding(ModifiedKey key);

			bool handleKeyInListeningState(ModifiedKey key);
			bool handleKeyInConflictState(ModifiedKey key);
			bool handleKeyInRequestResetState(ModifiedKey key);

		private:
			State _state = State::Listening;
			tgui::Button::Ptr _button;
			tgui::Group::Ptr _group;
			tgui::MessageBox::Ptr _popup;
			StateKeybindings& _bindings;
			const std::string& _actionName;
			BindingPosition _bindingPosition;
			std::string _keyName;
			unsigned int _onFocusHandlerID = 0;
			bool _isDirty = false;
		};

	private:
		void createKeybindingsGroup();
		void refreshKeybindingList(tgui::PanelListBox::Ptr list);
		void onKeybindSlotPressed(tgui::Button::Ptr button, tgui::Group::Ptr keybindingsGroup, StateKeybindings& bindings, const std::string& actionName, BindingPosition bindingPosition);

	private:
		tgui::PanelListBox::Ptr _keybindingList;
		std::optional<KeybindingListener> _keybindingListener;
	};
}