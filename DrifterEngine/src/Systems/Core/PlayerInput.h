#pragma once
#include "Systems/System.h"

namespace drft::action
{
	struct Action;
}

namespace drft::system
{
	struct KeyState
	{
		float timeHeld = 0;
		bool active = false;
	};

	class ActionMap
	{
	public:
		using emplaceFunc = std::function<void(entt::handle)>;
		void addAction(sf::Keyboard::Key key, emplaceFunc);
		bool contains(sf::Keyboard::Key key) const;
		emplaceFunc operator[](sf::Keyboard::Key key);
		std::unordered_map<sf::Keyboard::Key, emplaceFunc>& iterate();
	private:
		std::unordered_map<sf::Keyboard::Key, emplaceFunc> _map;
	};

	class PlayerInput : public System
	{
	public:
		using System::System;

		virtual void init() override;
		virtual void onUpdate(const float dt) override;

	private:
		std::unordered_map<sf::Keyboard::Key, KeyState> _keyState;
		ActionMap _actionMap;
		std::queue<ActionMap::emplaceFunc> _bufferedActions;
	};

}


