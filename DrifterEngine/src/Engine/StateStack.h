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
		explicit StateStack(StateContext context, tgui::Gui& gui);
		template<typename T>
		void registerState(States stateID)
		{
			_factories[stateID] = [this](tgui::Group::Ptr gui)
			{
				return State::StatePtr(new T(*this, _context, gui));
			};
		}

		void update(const float dt);
		void fixedUpdate();
		void render(sf::RenderTarget& target);
		void handleEvent(const sf::Event& event);

		void pushState(States stateID);
		void popState();
		void clearStates();

		bool isEmpty() const;

	private:
		State::StatePtr createState(States stateID);
		void applyPendingChanges();

		tgui::Group::Ptr createGuiGroup(States stateID);
		void disableGuiGroup(States stateID);

	private:
		struct PendingChange
		{
			Action action;
			States stateID;
		};
		struct StatePtrPair
		{
			States id;
			State::StatePtr ptr;
		};

	private:
		std::vector<StatePtrPair> _stack;
		std::vector<PendingChange> _pendingList;
		StateContext _context;
		tgui::Gui& _gui;
		std::unordered_map<States, std::function<State::StatePtr(tgui::Group::Ptr)>> _factories;
	};


}

