#pragma once
#include "Utility/ResourceHolder.h"
#include "StateContext.h"
#include "StateIdentifiers.h"

namespace drft
{
	class StateStack;

	class State
	{
	public:
		using StatePtr = std::unique_ptr<State>;

	public:
		State(StateStack& stack, StateContext& context, tgui::Group::Ptr gui);
		virtual ~State();

		virtual bool handleEvent(const sf::Event& ev);
		virtual bool update(const float dt);
		virtual bool fixedUpdate();
		virtual void render(sf::RenderTarget& target) {};

		virtual void onPush();
		virtual void onPop();

	protected:
		void requestStackPush(States stateId);
		void requestStackPop();
		void requestStackClear();

		const StateContext& getContext() const;
		StateContext& getContext();

	protected:
		tgui::Group::Ptr _gui;

	private:
		StateStack* _stack;
		StateContext _context;
	};
}



