#pragma once
#include <States/State.h>
#include <entt/entt.hpp>

namespace drft
{
	class AbilityScreenState : public State
	{
	public:
		AbilityScreenState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;

	private:
		void determineSessionEntity();
		void setupTemplateAbilityIcon(tgui::Panel::Ptr temp);
		void refreshAbilitiesIcons(tgui::HorizontalWrap::Ptr wrap);

		entt::handle _sessionEntity;
		tgui::Panel::Ptr _templateAbilityIcon;
	};
}