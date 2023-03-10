#include "pch.h"
#include "PlayerInput.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Actions/Move.h"
#include "Actions/ActionBuffer.h"

void drft::system::PlayerInput::init()
{
	using Key = sf::Keyboard;

	_actionMap.addAction(Key::Numpad1, new action::Move({ -1,  1 }));
	_actionMap.addAction(Key::Numpad2, new action::Move({  0,  1 }));
	_actionMap.addAction(Key::Numpad3, new action::Move({  1,  1 }));
	_actionMap.addAction(Key::Numpad4, new action::Move({ -1,  0 }));
	_actionMap.addAction(Key::Numpad5, new action::Move({  0,  0 }));
	_actionMap.addAction(Key::Numpad6, new action::Move({  1,  0 }));
	_actionMap.addAction(Key::Numpad7, new action::Move({ -1, -1 }));
	_actionMap.addAction(Key::Numpad8, new action::Move({  0, -1 }));
	_actionMap.addAction(Key::Numpad9, new action::Move({  1, -1 }));
	
}

void drft::system::PlayerInput::update(const float dt)
{
	auto view = registry->view<component::Player>(entt::exclude<component::Prototype>);

	for (auto [entity, player] : view.each())
	{
		for (auto&& [key, action] : _actionMap.iterate())
		{
			if (sf::Keyboard::isKeyPressed(key))
			{
				_keyState[key].active = false;
				if (_keyState[key].timeHeld <= std::numeric_limits<float>::epsilon())
				{
					// Just pressed
					_keyState[key].active = true;
					_keyState[key].timeHeld = 0.0f;
				}
				else if (_keyState[key].timeHeld >= _holdTime)
				{
					// Held key long enough
					_keyState[key].active = true;
					_keyState[key].timeHeld -= _refractoryPeriod;
				}
				
				_keyState[key].timeHeld += dt;
				if (_keyState[key].timeHeld > _holdTime)
				{
					_keyState[key].timeHeld = _holdTime;
				}
			}
			else
			{
				_keyState[key].timeHeld = 0.0f;
				_keyState[key].active = false;
			}

			if (_keyState[key].active)
			{
				player.actionBuffer.push(_actionMap[key]);
			}
		}
	}
}

void drft::system::ActionMap::addAction(sf::Keyboard::Key key, action::Action* action)
{
	_map[key] = std::unique_ptr<action::Action>(action);
}

std::unique_ptr<drft::action::Action> drft::system::ActionMap::operator[](sf::Keyboard::Key key)
{
	return _map[key]->clone();
}

std::unordered_map<sf::Keyboard::Key, std::unique_ptr<drft::action::Action>>& drft::system::ActionMap::iterate()
{
	return _map;
}

