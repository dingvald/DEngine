#pragma once
#include "Systems/System.h"

namespace drft::system
{
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
		virtual void update() override;

	private:
		ActionMap _actionMap;
	};

}


