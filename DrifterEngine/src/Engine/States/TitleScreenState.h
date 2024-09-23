#pragma once
#include "State.h"

namespace drft
{
	class TitleScreenState : public State
	{
	public:
		TitleScreenState(StateStack& stack, StateContext& context, tgui::Group::Ptr gui);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

	private:
		sf::Text _pressAnyButtonText;
	};
}


