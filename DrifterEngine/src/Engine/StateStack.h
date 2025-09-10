#pragma once
#include "States/StateIdentifiers.h"
#include "States/State.h"

namespace drft
{
	class StateStack
	{
	public:
		enum Action
		{
			Push,
			Pop,
			Clear
		};

	public:
		explicit StateStack(StateContext& context);

		template<typename T>
		void registerState(States stateID)
		{
			_factories[stateID] = [this]()
			{
				return State::StatePtr(new T(*this, _context));
			};
		}

		void update(const float dt);
		void fixedUpdate();
		void render(sf::RenderTarget& target);
		void guiRender(sf::RenderTarget& target);
		bool handleEvent(const sf::Event& event);

		void pushState(States stateID);
		void popState();
		void clearStates();
		void clearStatesNow();

		bool isEmpty() const;

	private:
		State::StatePtr createState(States stateID);
		void applyPendingChanges();

	private:
		struct PendingChange
		{
			Action action;
			States stateID;
		};

	private:
		std::vector<State::StatePtr> _stack;
		std::vector<PendingChange> _pendingList;
		StateContext& _context;
		std::unordered_map<States, std::function<State::StatePtr()>> _factories;
	};
}

