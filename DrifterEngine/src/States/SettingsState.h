#pragma once
#include <States/State.h>

namespace drft
{
	class SettingsState : public State
	{
	public:
		SettingsState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);

	private:
		void returnToSettingsList();
		void showSettingSelection(tgui::Group::Ptr group);

		void createSettingsListGroup();
		void createKeybindingsGroup();

	private:
		tgui::Group::Ptr _settingsListGroup;
		tgui::Group::Ptr _keybindingsGroup;

		bool _isShowingSettingsList = true;
	};
}