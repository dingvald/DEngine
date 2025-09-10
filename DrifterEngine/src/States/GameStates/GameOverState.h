#pragma once
#include <States/State.h>


namespace drft
{
	class GameOverState : public State
	{
	public:
		GameOverState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev);
		bool fixedUpdate() override;

	private:
		void updateBackgroundPanel() const;
		void updateCenterPanel() const;
		void updateText() const;

	private:
		float _elapsed = 0.f;
	};
}


