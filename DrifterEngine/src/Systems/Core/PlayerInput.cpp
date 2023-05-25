#include "pch.h"
#include "PlayerInput.h"
#include "Components/Components.h"
#include "Components/Tags.h"

static constexpr unsigned int INPUT_BUFFER_MAX_SIZE = 2;

void drft::system::PlayerInput::init()
{
	using Key = sf::Keyboard;

#pragma region Movement Actions
	_actionMap.addAction(Key::Numpad1, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(-1, 1)); 
		});
	_actionMap.addAction(Key::Numpad2, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(0, 1)); 
		});
	_actionMap.addAction(Key::Numpad3, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(1, 1)); 
		});
	_actionMap.addAction(Key::Numpad4, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(-1, 0)); 
		});
	_actionMap.addAction(Key::Numpad6, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(1, 0)); 
		});
	_actionMap.addAction(Key::Numpad7, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(-1, -1)); 
		});
	_actionMap.addAction(Key::Numpad8, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(0, -1)); 
		});
	_actionMap.addAction(Key::Numpad9, [](entt::handle entity) {
		entity.emplace<component::action::Move>(sf::Vector2i(1, -1)); 
		});
	_actionMap.addAction(Key::Numpad5, [](entt::handle entity) {
		entity.emplace<component::action::Wait>(); 
		});
#pragma endregion

	_actionMap.addAction(Key::G, [](entt::handle entity) {
		entity.emplace<component::action::PickUp>(); 
		});
	_actionMap.addAction(Key::I, [](entt::handle entity) {
		entity.emplace<component::action::OpenInventory>();
		});
	_actionMap.addAction(Key::E, [](entt::handle entity) {
		entity.emplace<component::action::OpenEquipment>();
		});
	_actionMap.addAction(Key::M, [](entt::handle entity) {
		entity.emplace<component::action::OpenWorldMap>();
		});
	_actionMap.addAction(Key::C, [](entt::handle entity) {
		entity.emplace<component::action::OpenCrafting>();
		});
	_actionMap.addAction(Key::S, [](entt::handle entity) {
		entity.emplace<component::action::ToggleSprint>();
		});
}

void drft::system::PlayerInput::update(const float dt)
{
	auto view = registry->view<component::Player>();
	for (auto entity : view)
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
				if (_bufferedActions[entity].size() < INPUT_BUFFER_MAX_SIZE)
				{
					_bufferedActions[entity].push(_actionMap[key]);
				}
			}
		}
	}

	auto turnView = registry->view<component::Player, component::tag::CurrentActor>();
	for (auto entity : turnView)
	{
		if (_bufferedActions[entity].empty()) continue;
		// emplaces buffered action component in entity
		_bufferedActions[entity].front()({ *registry, entity });
		_bufferedActions[entity].pop();
		if (_bufferedActions[entity].empty())
		{
			_bufferedActions.erase(entity);
		}
	}
}

void drft::system::ActionMap::addAction(sf::Keyboard::Key key, emplaceFunc func)
{
	_map[key] = func;
}

drft::system::ActionMap::emplaceFunc drft::system::ActionMap::operator[](sf::Keyboard::Key key)
{
	return _map.at(key);
}

std::unordered_map<sf::Keyboard::Key, drft::system::ActionMap::emplaceFunc>& drft::system::ActionMap::iterate()
{
	return _map;
}


