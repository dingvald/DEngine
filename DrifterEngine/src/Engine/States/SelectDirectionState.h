#pragma once
#include "State.h"
#include "GUI/GUIElement.h"
#include "Utility/SpriteBatch.h"

class TargetingParameters;

namespace drft
{
	class SelectDirectionState : public State
	{
	public:
		SelectDirectionState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;

		void onPush() override;
		void onPop() override;

	private:
		gui::Label _displayText;
	};
}


