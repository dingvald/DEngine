#include "pch.h"
#include "PlayerInput.h"
#include "Components/Components.h"
#include "Components/Tags.h"
#include "Systems/Helpers/InputBuffer.h"

static constexpr unsigned int INPUT_BUFFER_MAX_SIZE = 2;
static constexpr float REFRACTORY_PERIOD = 0.10f; // sec
static constexpr float HOLD_TIME = 0.5f; // sec

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
	_actionMap.addAction(Key::Space, [](entt::handle entity) {
		entity.emplace<component::action::TryInteract>();
		});
}

void drft::system::PlayerInput::update(const float dt)
{
	auto& inputBuffer = registry->ctx().get<InputBuffer&>();
	if (!inputBuffer.isEmpty())
	{
		const auto key = inputBuffer.popKey();
		if (_actionMap.contains(key))
		{
			_keyState[key].active = false;
			if (_keyState[key].timeHeld <= std::numeric_limits<float>::epsilon())
			{
				// Just pressed
				_keyState[key].active = true;
				_keyState[key].timeHeld = 0.0f;
			}
			else if (_keyState[key].timeHeld >= HOLD_TIME)
			{
				// Held key long enough
				_keyState[key].active = true;
				_keyState[key].timeHeld -= REFRACTORY_PERIOD;
			}

			_keyState[key].timeHeld += dt;
			if (_keyState[key].timeHeld > HOLD_TIME)
			{
				_keyState[key].timeHeld = HOLD_TIME;
			}

			if (_keyState[key].active && _bufferedActions.size() < INPUT_BUFFER_MAX_SIZE)
			{
				_bufferedActions.push(_actionMap[key]);
			}

			for (auto& [otherKey, keyState] : _keyState)
			{
				if (otherKey == key) continue;
				keyState.timeHeld = 0.0f;
				keyState.active = false;
			}
		}
	}
	else
	{
		for (auto& [otherKey, keyState] : _keyState)
		{
			keyState.timeHeld = 0.0f;
			keyState.active = false;
		}
	}

	auto turnView = registry->view<component::Player, component::tag::CurrentActor>();
	for (auto entity : turnView)
	{
		if (_bufferedActions.empty()) continue;
		_bufferedActions.front()({ *registry, entity });
		_bufferedActions.pop();
	}
}

void drft::system::ActionMap::addAction(sf::Keyboard::Key key, emplaceFunc func)
{
	_map[key] = func;
}

bool drft::system::ActionMap::contains(sf::Keyboard::Key key) const
{
	return _map.contains(key);
}

drft::system::ActionMap::emplaceFunc drft::system::ActionMap::operator[](sf::Keyboard::Key key)
{
	return _map.at(key);
}

std::unordered_map<sf::Keyboard::Key, drft::system::ActionMap::emplaceFunc>& drft::system::ActionMap::iterate()
{
	return _map;
}


