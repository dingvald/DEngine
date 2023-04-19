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
		using Ptr = std::unique_ptr<State>;

	public:
		State(StateStack& stack, StateContext& context);
		virtual ~State();

		virtual bool handleEvent(const sf::Event& ev);
		virtual bool update(const float dt) = 0;
		virtual void render(sf::RenderTarget& target) = 0;

		virtual void onPush();
		virtual void onPop();

	protected:
		void requestStackPush(States stateId);
		void requestStackPop();
		void requestStackClear();

		const StateContext& getContext() const;
		StateContext& getContext();

	private:
		StateStack* _stack;
		StateContext _context;
	};
}



