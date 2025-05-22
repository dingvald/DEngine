#pragma once
#include <States/State.h>
#include <Components/Wrappers/SkillsWrapper.h>


namespace drft
{
	class SkillsScreenState : public State
	{
	public:
		SkillsScreenState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);

	private:
		void determineSessionEntity();
		void setupPanelTemplate(tgui::Panel::Ptr templatePanel);
		void refreshSkillsList(tgui::PanelListBox::Ptr skillsList);
		void refreshAbilities(tgui::HorizontalWrap::Ptr abilities, entt::id_type skillId);

	private:
		SkillsWrapper _skills;

		entt::handle _sessionEntity;
	};
}