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
		State(StateStack& stack, StateContext& context);
		virtual ~State();

		virtual bool handleEvent(const sf::Event& ev);
		virtual bool update();
		virtual void render(sf::RenderTarget& target) {};
		virtual void guiRender(sf::RenderTarget& target) {};

		virtual void onEnter();
		virtual void onExit();
		virtual void onPush();
		virtual void onPop();

	protected:
		using FullyProcessed = std::true_type;

		void requestStackPush(States stateId);
		void requestStackPop();
		void requestStackClear();

		const StateContext& getContext() const;
		StateContext& getContext();

	protected:
		tgui::Group::Ptr _guiGroup;

	private:
		StateStack& _stack;
		StateContext& _context;
	};
}



