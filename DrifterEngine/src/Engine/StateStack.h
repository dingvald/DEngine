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
		explicit StateStack(State::Context context);
		template<typename T>
		void registerState(States stateID)
		{
			_factories[stateID] = [this]()
			{
				return State::Ptr(new T(*this, _context));
			};
		}

		void update(const float dt);
		void render(sf::RenderTarget& target);
		void handleEvent(const sf::Event& event);

		void pushState(States stateID);
		void popState();
		void clearStates();

		bool isEmpty() const;

	private:
		State::Ptr createState(States stateID);
		void applyPendingChanges();

	private:
		struct PendingChange
		{
			Action action;
			States stateID;
		};

	private:
		std::vector<State::Ptr> _stack;
		std::vector<PendingChange> _pendingList;
		State::Context _context;
		std::map<States, std::function<State::Ptr()>> _factories;
	};


}

